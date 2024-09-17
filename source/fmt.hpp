#pragma once

#include <concepts>
#include <string_view>

#include <fmt/base.h>

#include "twig/datadog/event.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
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

}  // namespace twig::datadog

template<twig::datadog::is_metric_like T>
struct fmt::formatter<T> : formatter<string_view>
{
    template<typename FormatContext>
    constexpr auto format(const T& metric, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "{}:{}|{}", metric.metric, metric.value, T::metric_type_indicator);
        if constexpr (twig::datadog::has_sample_rate<T>) {
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

template<>
struct fmt::formatter<twig::datadog::Event> : formatter<string_view>
{
    template<typename FormatContext>
    constexpr auto format(const twig::datadog::Event& event, FormatContext& ctx) const
    {
        auto out = fmt::format_to(
            ctx.out(), "_e{{{},{}}}:{}|{}", event.title.length(), event.text.length(), event.title, event.text);

        if (event.priority == twig::datadog::Priority::LOW) {
            out = fmt::format_to(out, "|p:low");
        }

        using enum twig::datadog::AlertType;
        switch (event.alert_type) {
            case ERROR:
                out = fmt::format_to(out, "|t:error");
                break;
            case WARNING:
                out = fmt::format_to(out, "|t:warning");
                break;
            case INFO:
                // This is assumed by default.
                break;
            case SUCCESS:
                out = fmt::format_to(out, "|t:success");
                break;
        }

        if (!event.tags.str().empty()) {
            out = fmt::format_to(out, "|#{}", event.tags.str());
        }

        return out;
    }
};
