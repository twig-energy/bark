#pragma once

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

class Client
{
    UDPClient _udp_client;

  public:
    explicit Client(UDPClient udp_client);

    auto send_async(const Datagram& datagram) -> void;
};

}  // namespace twig::datadog
