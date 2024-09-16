#include <cstddef>
#include <cstdint>
#include <functional>
#include <string_view>
#include <system_error>
#include <utility>

#include "twig/datadog/udp_server.hpp"

#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <fmt/base.h>

namespace twig::datadog
{

UDPServer::UDPServer(asio::io_context& io_context,
                     int32_t port,
                     std::function<void(std::string_view)> receive_msg_callback,
                     size_t buffer_size)
    : _socket(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), static_cast<uint16_t>(port)))
    , _receive_msg_callback(std::move(receive_msg_callback))
    , _recv_buffer(buffer_size)
{
    start_receive();
}

auto UDPServer::start_receive() -> void
{
    this->_socket.async_receive_from(asio::buffer(_recv_buffer),
                                     this->_latest_senders_endpoint,
                                     [this](const std::error_code& error, std::size_t bytes_transferred)
                                     { handle_receive(error, bytes_transferred); });
}

auto UDPServer::handle_receive(const std::error_code& error, std::size_t bytes_transferred) -> void
{
    // UDP we don't care about errors.
    if (!error) [[likely]] {
        this->_receive_msg_callback(std::string_view(this->_recv_buffer.data(), bytes_transferred));
    } else {
        fmt::println("Failed at receiving, maybe buffer is too small?");
    }
    start_receive();
}

}  // namespace twig::datadog
