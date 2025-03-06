#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "bark/transports/udp_transport.hpp"

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings

#include <asio/ip/udp.hpp>
#include <fmt/std.h>

#include "./sync_transport_common.hpp"
#include "bark/datagram.hpp"
#include "bark/tags.hpp"

namespace bark::transports
{

UDPTransport::UDPTransport(std::string_view host, uint16_t port, Tags global_tags)
    : _global_tags(std::move(global_tags))
    , _io_context(std::make_unique<asio::io_context>())
    , _receiver_endpoint(
          std::make_unique<asio::ip::udp::endpoint>(*asio::ip::udp::resolver(*this->_io_context)
                                                         .resolve(asio::ip::udp::v4(), host, std::to_string(port))
                                                         .begin()))
    , _socket(std::make_unique<asio::ip::udp::socket>(*this->_io_context))
{
    this->_socket->open(asio::ip::udp::v4());
    this->_socket->connect(*this->_receiver_endpoint);
}

auto UDPTransport::send(Datagram&& datagram) -> bool
{
    return sync_send(*this->_socket, std::move(datagram), this->_global_tags);
}

auto UDPTransport::make_local_udp_transport(uint16_t port, Tags global_tags) -> UDPTransport
{
    return {"localhost", port, std::move(global_tags)};
}

}  // namespace bark::transports
