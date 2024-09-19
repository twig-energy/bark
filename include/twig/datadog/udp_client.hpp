#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <string_view>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <asio/io_context.hpp>
#pragma GCC diagnostic pop
#include <asio/ip/udp.hpp>

namespace twig::datadog
{

class UDPClient
{
    constexpr static const uint16_t dogstatsd_udp_port = 8125;

    std::unique_ptr<asio::io_context> _io_context;
    asio::ip::udp::socket _socket;
    asio::ip::udp::endpoint _receiver_endpoint;

  public:
    UDPClient(std::string_view host, uint16_t port);

    auto send(std::string_view msg) -> bool;
    auto send_async(std::string_view msg) -> void;

    static auto make_local_udp_client(uint16_t port = dogstatsd_udp_port) -> UDPClient;
};

}  // namespace twig::datadog
