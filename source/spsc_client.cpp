#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stop_token>
#include <thread>
#include <utility>

#include "twig/datadog/spsc_client.hpp"

#include <rigtorp/SPSCQueue.h>

#include "twig/datadog/client.hpp"
#include "twig/datadog/datagram.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

SPSCClient::SPSCClient(UDPClient&& client_, std::size_t queue_size)
    : _queue(std::make_unique<rigtorp::SPSCQueue<Datagram>>(queue_size))
    , _worker(
          [queue_ptr = _queue.get(), client = Client(std::move(client_))](const std::stop_token& stop_token) mutable
          {
              try {
                  do {
                      while (!queue_ptr->empty()) {
                          client.send_async(*queue_ptr->front());
                          queue_ptr->pop();
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

auto SPSCClient::send_async(Datagram&& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    this->_queue->try_emplace(std::move(datagram));
}
auto SPSCClient::send_async(const Datagram& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    this->_queue->try_emplace(datagram);
}
}  // namespace twig::datadog
