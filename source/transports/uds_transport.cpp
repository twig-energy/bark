#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <utility>

#include "bark/transports/uds_transport.hpp"

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/local/datagram_protocol.hpp>
#include <fmt/std.h>

#include "./sync_transport_common.hpp"
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
    return sync_send(*this->_socket, std::move(datagram), this->_global_tags);
}

}  // namespace bark::transports

#endif  // BARK_UDS_ENABLED
