#pragma once

#include "feature_detection.hpp"

namespace bark
{

struct SampleRate
{
    double value;

    BARK_CONSTEXPR auto operator==(const SampleRate&) const -> bool = default;
};

}  // namespace bark
