#pragma once

#include <fmt/base.h>
#include <fmt/format.h>

#include "twig/datadog/count.hpp"

template<>
struct fmt::formatter<twig::datadog::Count> : formatter<string_view>
{
    template<typename FormatContext>
    constexpr auto format(const twig::datadog::Count& gauge, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "{}:{}|c", gauge.metric, gauge.value);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
        if (gauge.sample_rate != 1.0) {
            out = fmt::format_to(out, "|@{}", gauge.sample_rate);
        }
#pragma GCC diagnostic pop
        if (!gauge.tags.str().empty()) {
            out = fmt::format_to(out, "|#{}", gauge.tags.str());
        }

        return out;
    }
};
