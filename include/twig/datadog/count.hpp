#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "twig/datadog/sample_rate.hpp"
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

    constexpr auto with(SampleRate sample_rate_) -> Count&
    {
        if (sample_rate_.value < 0.0 || sample_rate_.value > 1.0) [[unlikely]] {
            throw std::invalid_argument("sample_rate must be in the range [0.0, 1.0]");
        }

        this->sample_rate = sample_rate_.value;
        return *this;
    }

    constexpr auto with(Tags tags_) -> Count&
    {
        this->tags = std::move(tags_);
        return *this;
    }

    auto serialize() const -> std::string;
};

}  // namespace twig::datadog
