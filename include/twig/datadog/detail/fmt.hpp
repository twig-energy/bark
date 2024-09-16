#pragma once

#include <concepts>
#include <string_view>

#include <fmt/base.h>

#include "twig/datadog/tags.hpp"

namespace twig::datadog::detail
{

template<typename T>
concept has_sample_rate = requires(T t) {
    {
        t.sample_rate
    } -> std::convertible_to<double>;
};

template<typename T>
concept is_metric_like = requires(T t) {
    {
        t.metric
    } -> std::convertible_to<std::string_view>;
    {
        t.value
    } -> std::convertible_to<double>;
    {
        t.tags
    } -> std::convertible_to<Tags>;
    {
        T::metric_type_indicator
    } -> std::convertible_to<std::string_view>;
};

}  // namespace twig::datadog::detail

template<twig::datadog::detail::is_metric_like T>
struct fmt::formatter<T> : formatter<string_view>
{
    template<typename FormatContext>
    constexpr auto format(const T& metric, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "{}:{}|{}", metric.metric, metric.value, T::metric_type_indicator);
        if constexpr (twig::datadog::detail::has_sample_rate<T>) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
            if (metric.sample_rate != 1.0) {
                out = fmt::format_to(out, "|@{}", metric.sample_rate);
            }
#pragma GCC diagnostic pop
        }
        if (!metric.tags.str().empty()) {
            out = fmt::format_to(out, "|#{}", metric.tags.str());
        }

        return out;
    }
};
