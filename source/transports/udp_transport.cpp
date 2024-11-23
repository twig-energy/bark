#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>
#include <system_error>

#include "bark/transports/udp_transport.hpp"
//
#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/buffer.hpp>
#include <asio/ip/udp.hpp>
#include <fmt/base.h>
#include <fmt/std.h>

namespace bark::transports
{

UDPTransport::UDPTransport(std::string_view host, uint16_t port)
    : _io_context(std::make_unique<asio::io_context>())
    , _receiver_endpoint(
          std::make_unique<asio::ip::udp::endpoint>(*asio::ip::udp::resolver(*this->_io_context)
                                                         .resolve(asio::ip::udp::v4(), host, std::to_string(port))
                                                         .begin()))
    , _socket(std::make_unique<asio::ip::udp::socket>(*this->_io_context))
{
    this->_socket->open(asio::ip::udp::v4());
    this->_socket->connect(*this->_receiver_endpoint);
}

auto UDPTransport::send(std::string_view msg) -> bool
{
    auto error = std::error_code {};
    auto bytes_sent = this->_socket->send(asio::buffer(msg), 0, error);
    if (error) [[unlikely]] {
        fmt::println(stderr, "Failed at sending {}. {}", error.message(), std::source_location::current());
    }
    return bytes_sent == msg.size();
}

auto UDPTransport::make_local_udp_transport(uint16_t port) -> UDPTransport
{
    return {"localhost", port};
}

}  // namespace bark::transports
