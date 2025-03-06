#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <utility>

#include "bark/transports/uds_transport.hpp"

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
    , _endpoint(std::make_unique<asio::local::datagram_protocol::endpoint>(socket_path.string()))
{
    this->_socket->open();
    this->_socket->connect(*this->_endpoint);
}

auto UDSTransport::send(Datagram&& datagram) -> bool
{
    return sync_send(*this->_socket, this->_global_tags, std::move(datagram));
}

}  // namespace bark::transports

#endif  // BARK_UDS_ENABLED
