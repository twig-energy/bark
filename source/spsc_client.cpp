#include <chrono>
#include <cstddef>
#include <stdexcept>
#include <stop_token>
#include <thread>
#include <utility>

#include "twig/datadog/spsc_client.hpp"

#include "twig/datadog/client.hpp"

namespace twig::datadog
{

SPSCClient::SPSCClient(Client client, std::size_t queue_size)
    : _queue(queue_size)
    , _client(std::move(client))
    , _worker(
          [this](const std::stop_token& stop_token)
          {
              try {
                  do {
                      while (!this->_queue.empty()) {
                          this->_client.send_async(*this->_queue.front());
                          this->_queue.pop();
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
