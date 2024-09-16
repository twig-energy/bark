#include <utility>
#include <variant>

#include "twig/datadog/client.hpp"

#include <fmt/format.h>

#include "twig/datadog/gauge_fmt.hpp"
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
        std::visit([](const auto& formattable_metric) { return fmt::format("{}", formattable_metric); }, metric));
}

}  // namespace twig::datadog
