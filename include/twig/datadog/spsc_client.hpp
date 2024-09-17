#pragma once

#include <cstddef>
#include <thread>

#include <rigtorp/SPSCQueue.h>

#include "twig/datadog/client.hpp"
#include "twig/datadog/datagram.hpp"

namespace twig::datadog
{

class SPSCClient
{
    rigtorp::SPSCQueue<Datagram> _queue;
    Client _client;
    std::jthread _worker;

  public:
    SPSCClient(Client udp_client, std::size_t queue_size);

    template<typename T>
    auto send_async(T&& value) -> void
    {
        // NOTE: try_emplace means that the metric will not be submitted if the queue is full.
        this->_queue.try_emplace(std::forward<T>(value));
    }
};

}  // namespace twig::datadog
