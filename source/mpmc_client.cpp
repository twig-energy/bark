#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <ostream>
#include <stop_token>
#include <thread>
#include <utility>

#include "bark/mpmc_client.hpp"

#include <rigtorp/MPMCQueue.h>

#include "bark/client.hpp"
#include "bark/datagram.hpp"
#include "bark/tags.hpp"
#include "bark/udp_client.hpp"

namespace bark
{

MPMCClient::MPMCClient(UDPClient&& udp_client, std::size_t queue_size, Tags global_tags)
    : _queue(std::make_unique<rigtorp::MPMCQueue<Datagram>>(queue_size))
    , _worker(
          [queue_ptr = _queue.get(),
           client = Client {std::move(udp_client), std::move(global_tags)}](const std::stop_token& stop_token) mutable
          {
              try {
                  auto popped = Datagram {Gauge("", 0)};  // Default value only present to avoid compilation error
                  while (!stop_token.stop_requested()) {
                      std::this_thread::sleep_for(std::chrono::milliseconds(1));

                      while (queue_ptr->try_pop(popped)) {
                          client.send(popped);
                      }
                  }
              } catch (const std::exception& ex) {
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
    auto copy = datagram;
    this->_queue->try_emplace(std::move(copy));
}
auto MPMCClient::send(Datagram&& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    this->_queue->try_emplace(std::move(datagram));
}

auto MPMCClient::make_local_client(std::size_t queue_size, Tags global_tags, uint16_t port) -> MPMCClient
{
    return {UDPClient::make_local_udp_client(port), queue_size, std::move(global_tags)};
}

}  // namespace bark
