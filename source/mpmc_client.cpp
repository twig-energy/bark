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

#include "twig/datadog/mpmc_client.hpp"

#include <rigtorp/MPMCQueue.h>

#include "twig/datadog/client.hpp"
#include "twig/datadog/datagram.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

MPMCClient::MPMCClient(UDPClient&& udp_client, std::size_t queue_size, Tags global_tags)
    : _queue(std::make_unique<rigtorp::MPMCQueue<Datagram>>(queue_size))
    , _worker(
          [is_running_ptr = this->_is_running.get(),
           queue_ptr = _queue.get(),
           client = Client {std::move(udp_client), std::move(global_tags)}](const std::stop_token& stop_token) mutable
          {
              try {
                  auto popped = Datagram {Gauge("", 0)};  // Default value only present to avoid compilation error
                  while (!stop_token.stop_requested()) {
                      std::this_thread::sleep_for(std::chrono::milliseconds(1));

                      while (!queue_ptr->empty()) {
                          if (!queue_ptr->try_pop(popped)) {
                              continue;
                          }

                          client.send(popped);
                      }
                  }
              } catch (const std::exception& ex) {
                  std::cerr << ex.what() << '\n' << std::flush;
                  // TODO(mikael): Log error
              }

              if (is_running_ptr != nullptr) {
                  is_running_ptr->store(false);
              }
          })
{
}

MPMCClient::~MPMCClient()
{
    if (this->_queue != nullptr && this->_is_running != nullptr) {
        this->flush();
    }
}

auto MPMCClient::flush() -> void
{
    // wait for the worker to finish processing the queue
    while (this->_is_running->load() && !this->_queue->empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
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

auto MPMCClient::make_local_client(std::size_t queue_size, Tags global_tags, uint16_t port) -> MPMCClient
{
    return {UDPClient::make_local_udp_client(port), queue_size, std::move(global_tags)};
}

}  // namespace twig::datadog
