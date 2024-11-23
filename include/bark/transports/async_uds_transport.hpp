#pragma once

#include <cstring>
#include <filesystem>
#include <memory>
#include <thread>
#include <vector>

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings
#include <asio/executor_work_guard.hpp>
#include <asio/local/datagram_protocol.hpp>

#include "bark/datagram.hpp"
#include "bark/number_of_io_threads.hpp"
#include "bark/tags.hpp"

namespace bark::transports
{

class AsyncUDSTransport
{
    std::unique_ptr<Tags> _global_tags;
    std::unique_ptr<asio::io_context> _io_context;
    std::unique_ptr<asio::local::datagram_protocol::endpoint> _endpoint;
    std::unique_ptr<asio::local::datagram_protocol::socket> _socket;
    std::vector<std::jthread> _io_threads;

    // This is registered after the threads, since it's destruction will allow the worker threads to stop when no more
    // work is in the queue.
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> _work_guard =
        std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(
            asio::make_work_guard(*this->_io_context));

  public:
    explicit AsyncUDSTransport(const std::filesystem::path& socket_path,
                               NumberOfIOThreads num_io_threads,
                               Tags global_tags = no_tags);

    auto send_async(Datagram&& datagram) -> void;
};

}  // namespace bark::transports
