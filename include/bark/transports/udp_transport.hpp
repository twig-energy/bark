#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <string_view>

#include "bark/asio_io_context_wrapper.hpp"
// ^ must be before asio includes, as it protects against gcc warnings
#include <asio/ip/udp.hpp>

#include "bark/transports/constants.hpp"

namespace bark::transports
{

class UDPTransport
{
    std::unique_ptr<asio::io_context> _io_context;
    std::unique_ptr<asio::ip::udp::endpoint> _receiver_endpoint;
    std::unique_ptr<asio::ip::udp::socket> _socket;

  public:
    UDPTransport(std::string_view host, uint16_t port);

    auto send(std::string_view msg) -> bool;

    static auto make_local_udp_transport(uint16_t port = dogstatsd_udp_port) -> UDPTransport;
};

}  // namespace bark::transports
