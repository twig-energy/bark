#pragma once

#include "twig/datadog/i_datadog_client.hpp"
#include "twig/datadog/metric.hpp"
#include "twig/datadog/udp_client.hpp"

namespace twig::datadog
{

struct NoOpClient : IDatadogClient
{
    explicit NoOpClient();

    auto send_async([[maybe_unused]] const Metric& metric) -> void override {}
    auto send_async([[maybe_unused]] Metric&& metric) -> void override {}
};

}  // namespace twig::datadog
