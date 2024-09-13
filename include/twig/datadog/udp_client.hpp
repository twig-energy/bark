#pragma once

#include <cstdint>
#include <cstring>
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
    asio::ip::udp::socket _socket;
    asio::ip::udp::endpoint _receiver_endpoint;

  public:
    UDPClient(asio::io_context& io_context, std::string_view host, uint16_t port);

    auto send(std::string_view msg) -> bool;

    auto send_async(std::string_view msg) -> void;
};

}  // namespace twig::datadog
