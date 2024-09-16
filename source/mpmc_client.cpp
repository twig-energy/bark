#include <chrono>
#include <cstddef>
#include <stdexcept>
#include <stop_token>
#include <thread>
#include <utility>

#include "twig/datadog/mpmc_client.hpp"

#include "twig/datadog/client.hpp"

namespace twig::datadog
{

MPMCClient::MPMCClient(Client client, std::size_t queue_size)
    : _queue(queue_size)
    , _client(std::move(client))
    , _worker(
          [this](const std::stop_token& stop_token)
          {
              try {
                  auto popped = Metric {Gauge("", 0)};  // Default value only present to avoid compilation error
                  do {
                      while (!this->_queue.empty()) {
                          if (!this->_queue.try_pop(popped)) {
                              continue;
                          }

                          this->_client.send_async(popped);
                      }

                      std::this_thread::sleep_for(std::chrono::milliseconds(1));
                  } while (!stop_token.stop_requested());
              } catch (const std::runtime_error& ex) {
                  // TODO(mikael): Log error
              }
          })
{
}

}  // namespace twig::datadog
