#pragma once

#include <string>
#include <utility>

#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

struct Gauge
{
    std::string metric;
    double value;
    double rate;
    Tags tags;

    // NOLINTNEXTLINE - consecutive parameters with same type
    constexpr Gauge(std::string metric_, double value_, double rate_ = 1.0, Tags tags_ = no_tags)
        : metric(std::move(metric_))
        , value(value_)
        , rate(rate_)
        , tags(std::move(tags_))
    {
    }
};

}  // namespace twig::datadog
