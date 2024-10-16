#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>
#include <system_error>

#include "bark/udp_client.hpp"

#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <fmt/base.h>
#include <fmt/std.h>

namespace bark
{

UDPClient::UDPClient(std::string_view host, uint16_t port)
    : _io_context(std::make_unique<asio::io_context>())
    , _receiver_endpoint(
          std::make_unique<asio::ip::udp::endpoint>(*asio::ip::udp::resolver(*this->_io_context)
                                                         .resolve(asio::ip::udp::v4(), host, std::to_string(port))
                                                         .begin()))
    , _socket(std::make_unique<asio::ip::udp::socket>(*this->_io_context))
{
    this->_socket->open(asio::ip::udp::v4());
}

auto UDPClient::send(std::string_view msg) -> bool
{
    auto error = std::error_code {};
    auto bytes_sent = this->_socket->send_to(asio::buffer(msg), *this->_receiver_endpoint, 0, error);
    if (error) [[unlikely]] {
        fmt::println(stderr, "Failed at sending {}. {}", error.message(), std::source_location::current());
    }
    return bytes_sent == msg.size();
}

auto UDPClient::make_local_udp_client(uint16_t port) -> UDPClient
{
    return {"localhost", port};
}

}  // namespace bark
