#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <variant>

#include "twig/datadog/asio_client.hpp"

#include <asio/buffer.hpp>
#include <asio/dispatch.hpp>
#include <asio/executor_work_guard.hpp>

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/tags.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <asio/io_context.hpp>
#pragma GCC diagnostic pop
#include <asio/ip/udp.hpp>
#include <fmt/base.h>

namespace twig::datadog
{

AsioClient::AsioClient(std::string_view host, uint16_t port, std::size_t num_io_threads, Tags global_tags)
    : _io_context(std::make_unique<asio::io_context>())
    , _socket(*this->_io_context)
    , _receiver_endpoint(
          *asio::ip::udp::resolver(*this->_io_context).resolve(asio::ip::udp::v4(), host, std::to_string(port)).begin())
    , _work_guard(std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(
          asio::make_work_guard(*this->_io_context)))
    , _global_tags(std::move(global_tags))
{
    if (num_io_threads == 0) {
        throw std::runtime_error("Cannot have 0 IO threads");
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

AsioClient::~AsioClient()
{
    // Allows the worker threads to stop when no more work is in the queue.
    this->_work_guard.reset();
}

auto AsioClient::send(const Datagram& datagram) -> void
{
    asio::dispatch(  //
        *this->_io_context,
        [this, message = datagram]() mutable
        {
            auto serialized = std::visit([this](const auto& serializable_datagram)
                                         { return serializable_datagram.serialize(this->_global_tags); },
                                         message);

            auto buffer = asio::buffer(serialized);
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

auto AsioClient::send(Datagram&& datagram) -> void
{
    asio::dispatch(  //
        *this->_io_context,
        [this, message = std::move(datagram)]() mutable
        {
            auto serialized = std::visit([this](const auto& serializable_datagram)
                                         { return serializable_datagram.serialize(this->_global_tags); },
                                         message);

            auto buffer = asio::buffer(serialized);
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

auto AsioClient::make_local_client(std::size_t num_io_threads, Tags global_tags, uint16_t port) -> AsioClient
{
    return {"localhost", port, num_io_threads, std::move(global_tags)};
}
}  // namespace twig::datadog
