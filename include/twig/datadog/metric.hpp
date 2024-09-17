#pragma once
#include <variant>

#include "twig/datadog/count.hpp"
#include "twig/datadog/gauge.hpp"
#include "twig/datadog/histogram.hpp"

namespace twig::datadog
{

using Metric = std::variant<Count, Gauge, Histogram>;

}  // namespace twig::datadog
