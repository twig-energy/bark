#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

#include <asio/io_context.hpp>
#include <asio/io_service.hpp>

#include "twig/datadog/udp_server.hpp"

namespace twig::datadog
{

class RAIIAsyncContext
{
    asio::io_context _io_context;
    std::jthread _runner;

  public:
    template<typename AsyncServerCreatorT>
    explicit RAIIAsyncContext(const AsyncServerCreatorT& server_creator_func)
    {
        auto mutex = std::mutex();
        auto cond_var = std::condition_variable();
        auto is_initialized = false;
        this->_runner = std::jthread(
            // The 3 referenced values are only used in this constructor. Therefore pass by ref is safe.
            // Specifically it is the 'is_initialized' and 'cond_var' which makes sure that the 'this->runner' thread is
            // at the io_context.run() call when the constructor is completed.
            [this, &server_creator_func, &is_initialized, &cond_var]
            {
                const auto work = asio::io_service::work(this->_io_context);
                auto server = server_creator_func(this->_io_context);
                is_initialized = true;
                cond_var.notify_one();  // now we can let the main thread continue
                this->_io_context.run();
            });

        // wait until the server has started
        std::unique_lock lk(mutex);
        cond_var.wait(lk, [&is_initialized]() { return is_initialized; });
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    RAIIAsyncContext(const RAIIAsyncContext&) = delete;
    RAIIAsyncContext(RAIIAsyncContext&& o) = delete;
    auto operator=(const RAIIAsyncContext&) -> RAIIAsyncContext& = delete;
    auto operator=(RAIIAsyncContext&& o) -> RAIIAsyncContext& = delete;

    auto stop() -> void
    {
        _io_context.stop();
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

}  // namespace twig::datadog
