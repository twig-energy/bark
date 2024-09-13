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
    Gauge(std::string_view metric, double value, double rate = 1.0, std::span<const std::string_view> tags = no_tags)
        : metric(metric)
        , value(value)
        , rate(rate)
        , tags(tags)
    {
    }
};

}  // namespace twig::datadog
