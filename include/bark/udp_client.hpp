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

namespace bark
{

constexpr static const uint16_t dogstatsd_udp_port = 8125;

class UDPClient
{
    std::unique_ptr<asio::io_context> _io_context;
    std::unique_ptr<asio::ip::udp::endpoint> _receiver_endpoint;
    std::unique_ptr<asio::ip::udp::socket> _socket;

  public:
    UDPClient(std::string_view host, uint16_t port);
    UDPClient(const UDPClient&) = delete;
    UDPClient(UDPClient&&) noexcept = default;
    auto operator=(const UDPClient&) -> UDPClient& = delete;
    auto operator=(UDPClient&&) noexcept -> UDPClient& = default;
    ~UDPClient();

    auto send(std::string_view msg) -> bool;

    static auto make_local_udp_client(uint16_t port = dogstatsd_udp_port) -> UDPClient;
};

}  // namespace bark
