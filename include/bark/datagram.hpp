#pragma once
#include <variant>

#include "bark/count.hpp"
#include "bark/event.hpp"
#include "bark/gauge.hpp"
#include "bark/histogram.hpp"

namespace bark
{

using Datagram = std::variant<Count, Event, Gauge, Histogram>;

}  // namespace bark
