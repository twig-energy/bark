#include <utility>
#include <variant>

#include "twig/datadog/client.hpp"

#include "twig/datadog/metric.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

Client::Client(UDPClient udp_client)
    : _udp_client(std::move(udp_client))
{
}

auto Client::send_async(const Metric& metric) -> void
{
    this->_udp_client.send_async(
        std::visit([](const auto& serializable_metric) { return serializable_metric.serialize(); }, metric));
}

}  // namespace twig::datadog
