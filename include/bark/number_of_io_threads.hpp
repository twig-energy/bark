#pragma once

#include <cstddef>

#include "feature_detection.hpp"

namespace bark
{

struct NumberOfIOThreads
{
    std::size_t value;

    BARK_CONSTEXPR auto operator==(const NumberOfIOThreads&) const -> bool = default;
};

}  // namespace bark
