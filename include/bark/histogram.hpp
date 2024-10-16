#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "bark/feature_detection.hpp"
#include "bark/sample_rate.hpp"
#include "bark/tags.hpp"

namespace bark
{

struct Histogram
{
    constexpr static std::string_view metric_type_indicator = "h";

    std::string metric;
    double value;
    double sample_rate = 1.0;
    Tags tags;

    BARK_CONSTEXPR Histogram(std::string metric_, double value_) noexcept
        : metric(std::move(metric_))
        , value(value_)
    {
    }

    constexpr auto with(SampleRate sample_rate_) -> Histogram&
    {
        if (sample_rate_.value < 0.0 || sample_rate_.value > 1.0) [[unlikely]] {
            throw std::invalid_argument("sample_rate must be in the range [0.0, 1.0]");
        }

        this->sample_rate = sample_rate_.value;
        return *this;
    }

    BARK_CONSTEXPR auto with(Tags tags_) -> Histogram&
    {
        this->tags = std::move(tags_);
        return *this;
    }

    auto serialize(const Tags& global_tags) const -> std::string;
};

}  // namespace bark
