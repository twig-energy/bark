#pragma once
#include <variant>

#include "twig/datadog/count.hpp"
#include "twig/datadog/event.hpp"
#include "twig/datadog/gauge.hpp"
#include "twig/datadog/histogram.hpp"

namespace twig::datadog
{

using Datagram = std::variant<Count, Event, Gauge, Histogram>;

}  // namespace twig::datadog
