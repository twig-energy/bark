#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <system_error>

#include "twig/datadog/udp_client.hpp"

#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <fmt/base.h>

namespace twig::datadog
{

UDPClient::UDPClient(asio::io_context& io_context, std::string_view host, uint16_t port)
    : _socket(io_context)
    , _receiver_endpoint(
          *asio::ip::udp::resolver(io_context).resolve(asio::ip::udp::v4(), host, std::to_string(port)).begin())
{
    this->_socket.open(asio::ip::udp::v4());
}

auto UDPClient::send(std::string_view msg) -> bool
{
    auto bytes_sent = this->_socket.send_to(asio::buffer(msg), _receiver_endpoint);
    return bytes_sent == msg.size();
}

auto UDPClient::send_async(std::string_view msg) -> void
{
    this->_socket.async_send_to(asio::buffer(msg),
                                this->_receiver_endpoint,
                                [](const std::error_code& error, std::size_t)
                                {
                                    if (error) [[unlikely]] {
                                        fmt::println("Failed at sending {}", error.message());
                                    }
                                });
}

}  // namespace twig::datadog
