#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

struct Histogram
{
    constexpr static std::string_view metric_type_indicator = "h";

    std::string metric;
    double value;
    double sample_rate = 1.0;
    Tags tags;

    constexpr Histogram(std::string metric_, double value_) noexcept
        : metric(std::move(metric_))
        , value(value_)
    {
    }

    constexpr auto with_sample_rate(double sample_rate_) -> Histogram&
    {
        if (sample_rate_ < 0.0 || sample_rate_ > 1.0) [[unlikely]] {
            throw std::invalid_argument("sample_rate must be in the range [0.0, 1.0]");
        }

        this->sample_rate = sample_rate_;
        return *this;
    }

    constexpr auto with_tags(Tags tags_) -> Histogram&
    {
        this->tags = std::move(tags_);
        return *this;
    }
};

}  // namespace twig::datadog
