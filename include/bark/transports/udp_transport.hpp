#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <string_view>

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings
#include <asio/ip/udp.hpp>

#include "bark/datagram.hpp"
#include "bark/tags.hpp"
#include "bark/transports/constants.hpp"

namespace bark::transports
{

class UDPTransport
{
    Tags _global_tags;
    std::unique_ptr<asio::io_context> _io_context;
    std::unique_ptr<asio::ip::udp::endpoint> _receiver_endpoint;
    std::unique_ptr<asio::ip::udp::socket> _socket;

  public:
    UDPTransport(std::string_view host, uint16_t port, Tags global_tags = no_tags);

    auto send(Datagram&& datagram) -> bool;

    static auto make_local_udp_transport(uint16_t port = dogstatsd_udp_port,
                                         Tags global_tags = no_tags) -> UDPTransport;
};

}  // namespace bark::transports
