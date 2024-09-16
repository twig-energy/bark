#pragma once

#include <fmt/base.h>
#include <fmt/format.h>

#include "twig/datadog/gauge.hpp"

template<>
struct fmt::formatter<twig::datadog::Gauge> : formatter<string_view>
{
    template<typename FormatContext>
    constexpr auto format(const twig::datadog::Gauge& gauge, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "{}:{}|g", gauge.metric, gauge.value);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
        if (gauge.rate != 1.0) {
            out = fmt::format_to(out, "|@{}", gauge.rate);
        }
#pragma GCC diagnostic pop
        if (!gauge.tags.str().empty()) {
            out = fmt::format_to(out, "|#{}", gauge.tags.str());
        }

        return out;
    }
};
