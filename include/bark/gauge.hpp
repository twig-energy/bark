#pragma once

#include <string>
#include <string_view>
#include <utility>

#include "bark/tags.hpp"

namespace bark
{

struct Gauge
{
    constexpr static std::string_view metric_type_indicator = "g";

    std::string metric;
    double value;
    Tags tags;

    constexpr Gauge(std::string metric_, double value_) noexcept
        : metric(std::move(metric_))
        , value(value_)
    {
    }

    constexpr auto with(Tags tags_) -> Gauge&
    {
        this->tags = std::move(tags_);
        return *this;
    }

    auto serialize(const Tags& global_tags) const -> std::string;
};

}  // namespace bark
