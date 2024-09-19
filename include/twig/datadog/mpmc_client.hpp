#pragma once

#include <cstddef>
#include <memory>
#include <thread>

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

class MPMCClient : public IDatadogClient
{
    std::unique_ptr<rigtorp::MPMCQueue<Datagram>> _queue;
    std::jthread _worker;

  public:
    MPMCClient(UDPClient&& udp_client, std::size_t queue_size);

    auto send(const Datagram& datagram) -> void override;
    auto send(Datagram&& datagram) -> void override;
};

}  // namespace twig::datadog
