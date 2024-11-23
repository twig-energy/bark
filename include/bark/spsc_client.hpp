#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <source_location>
#include <stop_token>
#include <thread>
#include <utility>

#include <fmt/base.h>
#include <fmt/std.h>
#include <rigtorp/SPSCQueue.h>

#include "bark/client.hpp"
#include "bark/datagram.hpp"
#include "bark/i_datadog_client.hpp"
#include "bark/tags.hpp"
#include "bark/transports/constants.hpp"
#include "bark/transports/datagram_transport.hpp"

namespace bark
{

class SPSCClient final : public IDatadogClient
{
    std::unique_ptr<rigtorp::SPSCQueue<Datagram>> _queue;
    std::jthread _worker;

  public:
    template<sync_datagram_transport Transport>

    SPSCClient(Transport&& transport, std::size_t queue_size)
        : _queue(std::make_unique<rigtorp::SPSCQueue<Datagram>>(queue_size))
        , _worker(
              [queue_ptr = this->_queue.get(), client = Client<Transport> {std::forward<Transport>(transport)}](
                  const std::stop_token& stop_token) mutable
              {
                  try {
                      while (!stop_token.stop_requested()) {
                          std::this_thread::sleep_for(std::chrono::milliseconds(1));

                          while (!queue_ptr->empty()) {
                              client.send(*queue_ptr->front());
                              queue_ptr->pop();
                          }
                      }
                  } catch (const std::exception& ex) {
                      fmt::println(stderr, "{}. {}", ex.what(), std::source_location::current());
                  }
              })
    {
    }

    auto send(Datagram&& datagram) -> void override;

    auto send(const Datagram& datagram) -> void override;

    static auto make_local_udp_client(std::size_t queue_size,
                                      Tags global_tags = no_tags,
                                      uint16_t port = transports::dogstatsd_udp_port) -> SPSCClient;
};

extern template SPSCClient::SPSCClient(transports::UDPTransport&&, std::size_t);
extern template SPSCClient::SPSCClient(transports::UDSTransport&&, std::size_t);

}  // namespace bark
