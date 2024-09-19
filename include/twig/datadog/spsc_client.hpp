#pragma once

#include <cstddef>
#include <memory>
#include <thread>

#include <rigtorp/SPSCQueue.h>

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/i_datadog_client.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

class SPSCClient : public IDatadogClient
{
    std::unique_ptr<rigtorp::SPSCQueue<Datagram>> _queue;
    std::jthread _worker;

  public:
    SPSCClient(UDPClient&& udp_client, std::size_t queue_size);

    auto send(const Datagram& datagram) -> void override;
    auto send(Datagram&& datagram) -> void override;

    template<typename T>
    constexpr auto send(T&& value) -> void
    {
        // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
        this->_queue->try_emplace(std::forward<T>(value));
    }
};

}  // namespace twig::datadog
