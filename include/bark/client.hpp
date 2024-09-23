#pragma once

#include <cstdint>

#include "bark/datagram.hpp"
#include "bark/i_datadog_client.hpp"
#include "bark/tags.hpp"
#include "bark/udp_client.hpp"

namespace bark
{

class Client final : public IDatadogClient
{
    UDPClient _udp_client;
    Tags _global_tags;

  public:
    explicit Client(UDPClient&& udp_client, Tags global_tags = no_tags);

    auto send(const Datagram& datagram) -> void override;
    auto send(Datagram&& datagram) -> void override;

    static auto make_local_client(Tags global_tags = no_tags, uint16_t port = dogstatsd_udp_port) -> Client;
};

}  // namespace bark
