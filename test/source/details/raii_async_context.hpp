#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/executor_work_guard.hpp>

#include "./udp_server.hpp"
#include "./uds_server.hpp"

namespace bark
{

class RAIIAsyncContext
{
    std::unique_ptr<asio::io_context> _io_context;
    std::unique_ptr<std::jthread> _runner;

    // This is registered after the thread, since it's destruction will allow the worker threads to stop when no more
    // work is in the queue.
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> _work_guard =
        std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(
            asio::make_work_guard(*this->_io_context));

  public:
    template<typename AsyncServerCreatorT>
    explicit RAIIAsyncContext(const AsyncServerCreatorT& server_creator_func)
        : _io_context(std::make_unique<asio::io_context>())
    {
        auto mutex = std::mutex();
        auto cond_var = std::condition_variable();
        auto is_initialized = false;
        this->_runner = std::make_unique<std::jthread>(
            // The 3 referenced values are only used in this constructor. Therefore pass by ref is safe.
            // Specifically it is the 'is_initialized' and 'cond_var' which makes sure that the 'this->runner' thread is
            // at the io_context.run() call when the constructor is completed.
            [io_context_ptr = this->_io_context.get(), &server_creator_func, &is_initialized, &cond_var]
            {
                auto server = server_creator_func(*io_context_ptr);
                is_initialized = true;
                cond_var.notify_one();  // now we can let the main thread continue
                io_context_ptr->run();
            });

        // wait until the server has started
        std::unique_lock lk(mutex);
        cond_var.wait(lk, [&is_initialized]() { return is_initialized; });
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    RAIIAsyncContext(const RAIIAsyncContext&) = delete;
    RAIIAsyncContext(RAIIAsyncContext&& o) = default;
    auto operator=(const RAIIAsyncContext&) -> RAIIAsyncContext& = delete;
    auto operator=(RAIIAsyncContext&& o) -> RAIIAsyncContext& = default;

    auto stop() -> void
    {
        if (this->_io_context != nullptr) {
            this->_io_context->stop();
        }
    }

    ~RAIIAsyncContext()
    {
        this->stop();
    }
};

template<typename ReceiveCallbackT>
auto make_local_udp_server(uint16_t port, ReceiveCallbackT callback)
{
    return RAIIAsyncContext([&](asio::io_context& io_context)
                            { return UDPServer(io_context, port, std::move(callback), 512); });
}

#if BARK_UDS_ENABLED

template<typename ReceiveCallbackT>
auto make_uds_server(std::filesystem::path socket_path, ReceiveCallbackT callback) -> RAIIAsyncContext
{
    return RAIIAsyncContext([&](asio::io_context& io_context)
                            { return UDSServer(io_context, socket_path, std::move(callback), 512); });
}

#endif  // BARK_UDS_ENABLED

}  // namespace bark
