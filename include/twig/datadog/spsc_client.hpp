#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <thread>

#include <rigtorp/SPSCQueue.h>

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/i_datadog_client.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

class SPSCClient final : public IDatadogClient
{
    std::unique_ptr<std::atomic<bool>> _is_running = std::make_unique<std::atomic<bool>>(true);
    std::unique_ptr<rigtorp::SPSCQueue<Datagram>> _queue;
    std::jthread _worker;

  public:
    SPSCClient(UDPClient&& udp_client, std::size_t queue_size, Tags global_tags = no_tags);
    SPSCClient(const SPSCClient&) = delete;
    SPSCClient(SPSCClient&& o) noexcept = default;
    auto operator=(const SPSCClient&) -> SPSCClient& = delete;
    auto operator=(SPSCClient&&) noexcept -> SPSCClient& = default;
    ~SPSCClient() override;

    auto send(const Datagram& datagram) -> void override;
    auto send(Datagram&& datagram) -> void override;

    auto flush() -> void;

    static auto make_local_client(std::size_t queue_size,
                                  Tags global_tags = no_tags,
                                  uint16_t port = dogstatsd_udp_port) -> SPSCClient;
};

}  // namespace twig::datadog
