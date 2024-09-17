#pragma once

#include "twig/datadog/datagram.hpp"
#include "twig/datadog/i_datadog_client.hpp"

namespace twig::datadog
{

struct NoOpClient : IDatadogClient
{
    explicit NoOpClient();

    auto send_async([[maybe_unused]] const Datagram& datagram) -> void override {}
    auto send_async([[maybe_unused]] Datagram&& datagram) -> void override {}
};

}  // namespace twig::datadog
