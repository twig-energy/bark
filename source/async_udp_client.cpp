#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

#include "twig/datadog/async_udp_client.hpp"

#include <asio/buffer.hpp>
#include <asio/dispatch.hpp>
#include <asio/executor_work_guard.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <asio/io_context.hpp>
#pragma GCC diagnostic pop
#include <asio/ip/udp.hpp>
#include <fmt/base.h>

namespace twig::datadog
{

AsyncUDPClient::AsyncUDPClient(std::string_view host, uint16_t port, std::size_t num_io_threads)
    : _io_context(std::make_unique<asio::io_context>())
    , _socket(*this->_io_context)
    , _receiver_endpoint(
          *asio::ip::udp::resolver(*this->_io_context).resolve(asio::ip::udp::v4(), host, std::to_string(port)).begin())
    , _work_guard(std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(
          asio::make_work_guard(*this->_io_context)))
{
    if (num_io_threads == 0) {
        throw std::invalid_argument("Cannot have 0 IO threads on AsyncUDPClient");
    }

    this->_socket.open(asio::ip::udp::v4());

    for (auto i = 0ULL; i < num_io_threads; i++) {
        this->_io_threads.emplace_back(
            [io_context_ptr = this->_io_context.get()](const std::stop_token& stop_token)
            {
                while (!stop_token.stop_requested()) {
                    io_context_ptr->run();
                }
            });
    }
}

AsyncUDPClient::~AsyncUDPClient()
{
    // Allows the worker threads to stop when no more work is in the queue.
    this->_work_guard.reset();
}

auto AsyncUDPClient::send(std::string msg) -> void
{
    asio::dispatch(  //
        *this->_io_context,
        [this, message = std::move(msg)]() mutable
        {
            auto buffer = asio::buffer(message);
            this->_socket.async_send_to(  //
                buffer,
                this->_receiver_endpoint,
                [](const std::error_code& error, std::size_t)
                {
                    if (error) {
                        fmt::print("Failed at sending {}\n", error.message());
                    }
                });
        });
}

auto AsyncUDPClient::make_local_udp_client(std::size_t num_io_threads, uint16_t port) -> AsyncUDPClient
{
    return {"localhost", port, num_io_threads};
}
}  // namespace twig::datadog
