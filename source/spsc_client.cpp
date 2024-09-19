#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <stop_token>
#include <thread>
#include <utility>

#include "twig/datadog/spsc_client.hpp"

#include <rigtorp/SPSCQueue.h>

#include "twig/datadog/client.hpp"
#include "twig/datadog/datagram.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

SPSCClient::SPSCClient(UDPClient&& udp_client, std::size_t queue_size, Tags global_tags)
    : _queue(std::make_unique<rigtorp::SPSCQueue<Datagram>>(queue_size))
    , _worker(
          [is_running_ptr = this->_is_running.get(),
           queue_ptr = this->_queue.get(),
           client = Client {std::move(udp_client), std::move(global_tags)}](const std::stop_token& stop_token) mutable
          {
              try {
                  while (!stop_token.stop_requested()) {
                      while (!queue_ptr->empty()) {
                          client.send(*queue_ptr->front());
                          queue_ptr->pop();
                      }

                      std::this_thread::sleep_for(std::chrono::milliseconds(1));
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

SPSCClient::~SPSCClient()
{
    if (this->_queue != nullptr && this->_is_running != nullptr) {
        this->flush();
    }
}

auto SPSCClient::flush() -> void
{
    // wait for the worker to finish processing the queue
    while (this->_is_running->load() && !this->_queue->empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

auto SPSCClient::send(Datagram&& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    this->_queue->try_emplace(std::move(datagram));
}

auto SPSCClient::send(const Datagram& datagram) -> void
{
    // NOTE: try_emplace means that the datagram will not be submitted if the queue is full.
    this->_queue->try_emplace(datagram);
}

auto SPSCClient::make_local_client(std::size_t queue_size, Tags global_tags, uint16_t port) -> SPSCClient
{
    return {UDPClient::make_local_udp_client(port), queue_size, std::move(global_tags)};
}

}  // namespace twig::datadog
