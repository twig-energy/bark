#pragma once

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/i_datadog_client.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

class Client : public IDatadogClient
{
    UDPClient _udp_client;
    Tags _global_tags;

  public:
    explicit Client(UDPClient&& udp_client);
    Client(UDPClient&& udp_client, Tags global_tags);

    auto send_async(const Datagram& datagram) -> void override;
    auto send_async(Datagram&& datagram) -> void override;
};

}  // namespace twig::datadog
