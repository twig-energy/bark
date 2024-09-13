#pragma once

#include <array>
#include <span>
#include <string_view>

#include "twig/datadog/compile_time_string.hpp"

namespace twig::datadog
{

constexpr const auto no_tags = std::array<CompileTimeString, 0> {};

struct Gauge
{
    CompileTimeString metric;
    double value;
    double rate;
    std::span<const CompileTimeString> tags;

    // NOLINTNEXTLINE - consecutive parameters with same type
    Gauge(CompileTimeString metric_,
          double value_,
          double rate_ = 1.0,
          std::span<const CompileTimeString> tags_ = no_tags)
        : metric(metric_)
        , value(value_)
        , rate(rate_)
        , tags(tags_)
    {
    }
};

}  // namespace twig::datadog
