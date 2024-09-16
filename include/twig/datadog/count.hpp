#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

struct Count
{
    constexpr static std::string_view metric_type_indicator = "c";

    std::string metric;
    int32_t value;
    double sample_rate = 1.0;
    Tags tags;

    constexpr Count(std::string metric_, int32_t value_) noexcept
        : metric(std::move(metric_))
        , value(value_)
    {
    }

    constexpr auto with_sample_rate(double sample_rate_) -> Count&
    {
        if (sample_rate_ < 0.0 || sample_rate_ > 1.0) {
            throw std::invalid_argument("sample_rate must be in the range [0.0, 1.0]");
        }

        this->sample_rate = sample_rate_;
        return *this;
    }

    constexpr auto with_tags(Tags tags_) -> Count&
    {
        this->tags = std::move(tags_);
        return *this;
    }
};

}  // namespace twig::datadog
