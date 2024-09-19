#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <thread>

#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

// TODO(mikael): Keep an eye on https://github.com/rigtorp/MPMCQueue/issues/49 to see if this is still needed
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic push
#include <rigtorp/MPMCQueue.h>
#pragma GCC diagnostic pop

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/i_datadog_client.hpp"

namespace twig::datadog
{

class MPMCClient final : public IDatadogClient
{
    std::unique_ptr<std::atomic<bool>> _is_running = std::make_unique<std::atomic<bool>>(true);
    std::unique_ptr<rigtorp::MPMCQueue<Datagram>> _queue;
    std::jthread _worker;

  public:
    MPMCClient(UDPClient&& udp_client, std::size_t queue_size, Tags global_tags = no_tags);
    MPMCClient(const MPMCClient&) = delete;
    MPMCClient(MPMCClient&&) noexcept = default;
    auto operator=(const MPMCClient&) -> MPMCClient& = delete;
    auto operator=(MPMCClient&&) noexcept -> MPMCClient& = default;
    ~MPMCClient() override;

    auto send(const Datagram& datagram) -> void override;
    auto send(Datagram&& datagram) -> void override;

    auto flush() -> void;

    static auto make_local_client(std::size_t queue_size,
                                  Tags global_tags = no_tags,
                                  uint16_t port = dogstatsd_udp_port) -> MPMCClient;
};

}  // namespace twig::datadog
