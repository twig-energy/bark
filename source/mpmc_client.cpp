#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stop_token>
#include <thread>
#include <utility>

#include "twig/datadog/mpmc_client.hpp"

#include <rigtorp/MPMCQueue.h>

#include "twig/datadog/client.hpp"
#include "twig/datadog/datagram.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

MPMCClient::MPMCClient(UDPClient&& client, std::size_t queue_size)
    : _queue(std::make_unique<rigtorp::MPMCQueue<Datagram>>(queue_size))
    , _worker(
          [queue_ptr = _queue.get(), _client = Client(std::move(client))](const std::stop_token& stop_token) mutable
          {
              try {
                  auto popped = Datagram {Gauge("", 0)};  // Default value only present to avoid compilation error
                  do {
                      while (!queue_ptr->empty()) {
                          if (!queue_ptr->try_pop(popped)) {
                              continue;
                          }

                          _client.send_async(popped);
                      }

                      std::this_thread::sleep_for(std::chrono::milliseconds(1));
                  } while (!stop_token.stop_requested());
              } catch (const std::runtime_error& ex) {
                  std::cerr << ex.what() << std::endl;
                  // TODO(mikael): Log error
              }
          })
{
}

auto MPMCClient::send_async(const Datagram& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    // unfortunately we have to copy datagram because std::variant is not nothrow copy constructible
    this->_queue->try_emplace(auto(datagram));
}
auto MPMCClient::send_async(Datagram&& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    this->_queue->try_emplace(std::move(datagram));
}

}  // namespace twig::datadog
