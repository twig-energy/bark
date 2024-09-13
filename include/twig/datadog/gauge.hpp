#pragma once

#include <array>
#include <span>
#include <string_view>

namespace twig::datadog
{

constexpr const auto no_tags = std::array<std::string_view, 0> {};

struct Gauge
{
    std::string_view metric;
    double value;
    double rate;
    std::span<const std::string_view> tags;

    // NOLINTNEXTLINE - consecutive parameters with same type
    Gauge(std::string_view metric_,
          double value_,
          double rate_ = 1.0,
          std::span<const std::string_view> tags_ = no_tags)
        : metric(metric_)
        , value(value_)
        , rate(rate_)
        , tags(tags_)
    {
    }
};

}  // namespace twig::datadog
