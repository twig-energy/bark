#pragma once
#include <variant>

#include "twig/datadog/gauge.hpp"

namespace twig::datadog
{

using Metric = std::variant<Gauge>;

}  // namespace twig::datadog
