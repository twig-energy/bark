#pragma once

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/i_datadog_client.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

class Client : public IDatadogClient
{
    UDPClient _udp_client;

  public:
    explicit Client(UDPClient&& udp_client);

    auto send_async(const Datagram& metric) -> void override;
    auto send_async(Datagram&& metric) -> void override;
};

}  // namespace twig::datadog
