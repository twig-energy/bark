#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>

#include "twig/datadog/udp_client.hpp"

#include <asio/buffer.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <asio/io_context.hpp>
#pragma GCC diagnostic pop
#include <asio/ip/udp.hpp>
#include <fmt/base.h>

namespace twig::datadog
{

UDPClient::UDPClient(std::string_view host, uint16_t port)
    : _io_context(std::make_unique<asio::io_context>())
    , _socket(*this->_io_context)
    , _receiver_endpoint(
          *asio::ip::udp::resolver(*this->_io_context).resolve(asio::ip::udp::v4(), host, std::to_string(port)).begin())
{
    this->_socket.open(asio::ip::udp::v4());
}

auto UDPClient::send(std::string_view msg) -> bool
{
    auto error = std::error_code {};
    auto bytes_sent = this->_socket.send_to(asio::buffer(msg), _receiver_endpoint, 0, error);
    if (error) [[unlikely]] {
        fmt::println("Failed at sending {}", error.message());
    }
    return bytes_sent == msg.size();
}

auto UDPClient::make_local_udp_client(uint16_t port) -> UDPClient
{
    return {"localhost", port};
}
}  // namespace twig::datadog
