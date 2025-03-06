#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <source_location>
#include <system_error>
#include <utility>
#include <variant>

#include "bark/transports/uds_transport.hpp"

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/buffer.hpp>
#include <asio/local/datagram_protocol.hpp>
#include <fmt/base.h>
#include <fmt/std.h>

#include "bark/datagram.hpp"
#include "bark/feature_detection.hpp"
#include "bark/tags.hpp"

#if BARK_UDS_ENABLED

namespace bark::transports
{

UDSTransport::UDSTransport(const std::filesystem::path& socket_path, Tags global_tags)
    : _global_tags(std::move(global_tags))
    , _io_context(std::make_unique<asio::io_context>())
    , _endpoint(std::make_unique<asio::local::datagram_protocol::endpoint>(socket_path.string()))
    , _socket(std::make_unique<asio::local::datagram_protocol::socket>(*this->_io_context))
{
    this->_socket->open();
    this->_socket->connect(*this->_endpoint);
}

auto UDSTransport::send(Datagram&& datagram) -> bool
{
    auto serialized = std::visit([this](const auto& serializable_datagram)
                                 { return serializable_datagram.serialize(this->_global_tags); },
                                 std::move(datagram));

    auto error = std::error_code {};
    auto bytes_sent = this->_socket->send(asio::buffer(serialized), 0, error);
    if (error) [[unlikely]] {
        fmt::println(stderr, "Failed at sending {}. {}", error.message(), std::source_location::current());
    }
    return bytes_sent == serialized.size();
}

}  // namespace bark::transports

#endif  // BARK_UDS_ENABLED
