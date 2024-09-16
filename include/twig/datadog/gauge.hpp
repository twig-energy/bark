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
    Tags tags;

    constexpr Gauge(std::string metric_, double value_) noexcept
        : metric(std::move(metric_))
        , value(value_)
    {
    }

    constexpr auto with_tags(Tags tags_) -> Gauge&
    {
        this->tags = std::move(tags_);
        return *this;
    }
};

}  // namespace twig::datadog
