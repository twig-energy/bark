#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <string_view>
#include <thread>
#include <vector>

#include <asio/executor_work_guard.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <asio/io_context.hpp>
#pragma GCC diagnostic pop
#include <asio/ip/udp.hpp>

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/i_datadog_client.hpp"
#include "twig/datadog/number_of_io_threads.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

class AsioClient final : public IDatadogClient
{
    std::unique_ptr<Tags> _global_tags;
    std::unique_ptr<asio::io_context> _io_context;
    std::unique_ptr<asio::ip::udp::socket> _socket = std::make_unique<asio::ip::udp::socket>(*this->_io_context);
    std::unique_ptr<asio::ip::udp::endpoint> _receiver_endpoint;
    std::vector<std::jthread> _io_threads;

    // This is registered after the threads, since it's destruction will allow the worker threads to stop when no more
    // work is in the queue.
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> _work_guard =
        std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(
            asio::make_work_guard(*this->_io_context));

  public:
    AsioClient(std::string_view host, uint16_t port, NumberOfIOThreads num_io_threads, Tags global_tags = no_tags);

    auto send(const Datagram& datagram) -> void override;
    auto send(Datagram&& datagram) -> void override;

    static auto make_local_client(NumberOfIOThreads num_io_threads,
                                  Tags global_tags = no_tags,
                                  uint16_t port = dogstatsd_udp_port) -> AsioClient;
};

}  // namespace twig::datadog
