#pragma once

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/i_datadog_client.hpp"

namespace twig::datadog
{

struct NoOpClient : IDatadogClient
{
    auto send([[maybe_unused]] const Datagram& datagram) -> void override {}
    auto send([[maybe_unused]] Datagram&& datagram) -> void override {}
};

}  // namespace twig::datadog
