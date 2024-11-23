#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <stop_token>
#include <thread>
#include <utility>

#include "bark/client.hpp"
#include "bark/feature_detection.hpp"
#include "bark/transports/constants.hpp"
#include "bark/transports/datagram_transport.hpp"

#if BARK_GCC_VERSION > 0 || BARK_CLANG_VERSION > 0
// TODO(mikael): Keep an eye on https://github.com/rigtorp/MPMCQueue/issues/49 to see if this is still needed
#    pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#    pragma GCC diagnostic push
#endif
#include <rigtorp/MPMCQueue.h>
#if BARK_GCC_VERSION > 0 || BARK_CLANG_VERSION > 0
#    pragma GCC diagnostic pop
#endif

#include "bark/datagram.hpp"
#include "bark/i_datadog_client.hpp"
#include "bark/tags.hpp"

namespace bark
{

class MPMCClient final : public IDatadogClient
{
    std::unique_ptr<rigtorp::MPMCQueue<Datagram>> _queue;
    std::jthread _worker;

  public:
    template<sync_datagram_transport Transport>
    MPMCClient(Transport&& transport, std::size_t queue_size, Tags global_tags = no_tags)
        : _queue(std::make_unique<rigtorp::MPMCQueue<Datagram>>(queue_size))
        , _worker(
              [queue_ptr = _queue.get(),
               client = Client<Transport> {std::forward<Transport>(transport), std::move(global_tags)}](
                  const std::stop_token& stop_token) mutable
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

    auto send(const Datagram& datagram) -> void override;
    auto send(Datagram&& datagram) -> void override;

    static auto make_local_udp_client(std::size_t queue_size,
                                      Tags global_tags = no_tags,
                                      uint16_t port = transports::dogstatsd_udp_port) -> MPMCClient;
};

extern template MPMCClient::MPMCClient(transports::UDPTransport&&, std::size_t, Tags);
extern template MPMCClient::MPMCClient(transports::UDSTransport&&, std::size_t, Tags);

}  // namespace bark
