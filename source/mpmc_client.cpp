#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <ostream>
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

MPMCClient::MPMCClient(UDPClient&& udp_client, std::size_t queue_size)
    : _queue(std::make_unique<rigtorp::MPMCQueue<Datagram>>(queue_size))
    , _worker(
          [queue_ptr = _queue.get(), client = Client(std::move(udp_client))](const std::stop_token& stop_token) mutable
          {
              try {
                  auto popped = Datagram {Gauge("", 0)};  // Default value only present to avoid compilation error
                  while (!stop_token.stop_requested()) {
                      while (!queue_ptr->empty()) {
                          if (!queue_ptr->try_pop(popped)) {
                              continue;
                          }

                          client.send(popped);
                      }

                      std::this_thread::sleep_for(std::chrono::milliseconds(1));
                  }
              } catch (const std::runtime_error& ex) {
                  std::cerr << ex.what() << '\n' << std::flush;
                  // TODO(mikael): Log error
              }
          })
{
}

auto MPMCClient::send(const Datagram& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    // unfortunately we have to copy datagram because std::variant is not nothrow copy constructible
    this->_queue->try_emplace(auto(datagram));
}
auto MPMCClient::send(Datagram&& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    this->_queue->try_emplace(std::move(datagram));
}

}  // namespace twig::datadog
