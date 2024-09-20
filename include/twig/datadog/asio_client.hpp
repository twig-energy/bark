#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <string_view>
#include <thread>
#include <vector>

#include <asio/executor_work_guard.hpp>

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/i_datadog_client.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <asio/io_context.hpp>
#pragma GCC diagnostic pop
#include <asio/ip/udp.hpp>

namespace twig::datadog
{

class AsioClient final : IDatadogClient
{
    std::unique_ptr<asio::io_context> _io_context;
    asio::ip::udp::socket _socket;
    asio::ip::udp::endpoint _receiver_endpoint;
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> _work_guard;
    std::vector<std::jthread> _io_threads;
    Tags _global_tags;

  public:
    AsioClient(std::string_view host, uint16_t port, std::size_t num_io_threads, Tags global_tags = no_tags);
    AsioClient(const AsioClient&) = delete;
    AsioClient(AsioClient&& other) noexcept = default;
    auto operator=(const AsioClient&) -> AsioClient& = delete;
    auto operator=(AsioClient&& other) noexcept -> AsioClient& = default;
    ~AsioClient() override;

    auto send(const Datagram& datagram) -> void override;
    auto send(Datagram&& datagram) -> void override;

    static auto make_local_client(std::size_t num_io_threads,
                                  Tags global_tags = no_tags,
                                  uint16_t port = dogstatsd_udp_port) -> AsioClient;
};

}  // namespace twig::datadog
