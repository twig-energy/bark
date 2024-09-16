#pragma once
#include <variant>

#include "twig/datadog/count.hpp"
#include "twig/datadog/gauge.hpp"

namespace twig::datadog
{

using Metric = std::variant<Count, Gauge>;

}  // namespace twig::datadog
