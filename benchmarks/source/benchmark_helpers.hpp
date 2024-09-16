#pragma once

#include <algorithm>
#include <cstddef>
#include <random>
#include <vector>

namespace twig::datadog
{

inline auto random_double_vector(std::size_t elements, double min, double max) -> std::vector<double>
{
    static auto rd = std::random_device();
    static auto gen = std::mt19937(rd());

    auto values = std::vector<double>(elements);
    auto dis = std::uniform_real_distribution<>(min, max);
    std::ranges::generate(values, [&]() { return dis(gen); });

    return values;
}

}  // namespace twig::datadog
