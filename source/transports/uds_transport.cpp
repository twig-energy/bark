#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <source_location>
#include <string_view>
#include <system_error>

#include "bark/transports/uds_transport.hpp"

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/buffer.hpp>
#include <asio/local/datagram_protocol.hpp>
#include <fmt/base.h>
#include <fmt/std.h>

namespace bark::transports
{

UDSTransport::UDSTransport(const std::filesystem::path& socket_path)
    : _io_context(std::make_unique<asio::io_context>())
    , _endpoint(std::make_unique<asio::local::datagram_protocol::endpoint>(socket_path.c_str()))
    , _socket(std::make_unique<asio::local::datagram_protocol::socket>(*this->_io_context))
{
    this->_socket->open();
    this->_socket->connect(*this->_endpoint);
}

auto UDSTransport::send(std::string_view msg) -> bool
{
    auto error = std::error_code {};
    auto bytes_sent = this->_socket->send(asio::buffer(msg), 0, error);
    if (error) [[unlikely]] {
        fmt::println(stderr, "Failed at sending {}. {}", error.message(), std::source_location::current());
    }
    return bytes_sent == msg.size();
}

}  // namespace bark::transports
