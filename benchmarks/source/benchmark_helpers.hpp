#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

namespace twig::datadog
{

namespace
{

static auto rd = std::random_device();
static auto gen = std::mt19937(rd());

}  // namespace

inline auto random_double_vector(std::size_t elements, double min, double max) -> std::vector<double>
{
    auto values = std::vector<double>(elements);
    auto dis = std::uniform_real_distribution<>(min, max);
    std::ranges::generate(values, [&]() { return dis(gen); });

    return values;
}

inline auto random_int32_t_vector(std::size_t elements, int32_t min, int32_t max) -> std::vector<int32_t>
{
    auto values = std::vector<int32_t>(elements);
    auto dis = std::uniform_int_distribution<int32_t>(min, max);
    std::ranges::generate(values, [&]() { return dis(gen); });

    return values;
}

}  // namespace twig::datadog
