#pragma once

#include <cstddef>
#include <thread>

// TODO(mikael): Keep an eye on https://github.com/rigtorp/MPMCQueue/issues/49 to see if this is still needed
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic push
#include <rigtorp/MPMCQueue.h>
#pragma GCC diagnostic pop

#include "twig/datadog/client.hpp"
#include "twig/datadog/datagram.hpp"

namespace twig::datadog
{

class MPMCClient
{
    rigtorp::MPMCQueue<Datagram> _queue;
    Client _client;
    std::jthread _worker;

  public:
    MPMCClient(Client udp_client, std::size_t queue_size);

    template<typename T>
    auto send_async(T&& value) -> void
    {
        // NOTE: try_emplace means that the metric will not be submitted if the queue is full.
        this->_queue.try_emplace(std::forward<T>(value));
    }
};

}  // namespace twig::datadog
