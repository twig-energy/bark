#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <thread>

#include <rigtorp/SPSCQueue.h>

#include "bark/datagram.hpp"
#include "bark/i_datadog_client.hpp"
#include "bark/tags.hpp"
#include "bark/udp_client.hpp"

namespace bark
{

class SPSCClient final : public IDatadogClient
{
    std::unique_ptr<rigtorp::SPSCQueue<Datagram>> _queue;
    std::jthread _worker;

  public:
    SPSCClient(UDPClient&& udp_client, std::size_t queue_size, Tags global_tags = no_tags);

    auto send(const Datagram& datagram) -> void override;
    auto send(Datagram&& datagram) -> void override;

    static auto make_local_client(std::size_t queue_size,
                                  Tags global_tags = no_tags,
                                  uint16_t port = dogstatsd_udp_port) -> SPSCClient;
};

}  // namespace bark
