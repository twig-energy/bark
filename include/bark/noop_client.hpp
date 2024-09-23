#pragma once

#include "bark/datagram.hpp"
#include "bark/i_datadog_client.hpp"

namespace bark
{

struct NoOpClient final : IDatadogClient
{
    auto send([[maybe_unused]] const Datagram& datagram) -> void override {}
    auto send([[maybe_unused]] Datagram&& datagram) -> void override {}
};

}  // namespace bark
