#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <string_view>
#include <thread>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <asio/io_context.hpp>
#pragma GCC diagnostic pop
#include <asio/ip/udp.hpp>

namespace twig::datadog
{

constexpr static const uint16_t dogstatsd_udp_port = 8125;

class AsyncUDPClient
{
    std::unique_ptr<asio::io_context> _io_context;
    asio::ip::udp::socket _socket;
    asio::ip::udp::endpoint _receiver_endpoint;
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> _work_guard;
    std::vector<std::jthread> _io_threads;

  public:
    AsyncUDPClient(std::string_view host, uint16_t port, std::size_t num_io_threads);
    AsyncUDPClient(const AsyncUDPClient&) = delete;
    AsyncUDPClient(AsyncUDPClient&& other) noexcept = default;
    ~AsyncUDPClient();

    auto operator=(const AsyncUDPClient&) -> AsyncUDPClient& = delete;
    auto operator=(AsyncUDPClient&& other) noexcept -> AsyncUDPClient& = default;

    auto send(std::string_view msg) -> void;

    static auto make_local_udp_client(std::size_t num_io_threads, uint16_t port = dogstatsd_udp_port) -> AsyncUDPClient;
};

}  // namespace twig::datadog
