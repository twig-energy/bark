#pragma once

#include <fmt/base.h>
#include <fmt/format.h>

#include "twig/datadog/compile_time_string.hpp"

template<>
struct fmt::formatter<twig::datadog::CompileTimeString> : formatter<string_view>
{
    template<typename FormatContext>
    constexpr auto format(const twig::datadog::CompileTimeString& string, FormatContext& ctx) const
    {
        return formatter<string_view>::format(string.value, ctx);
    }
};
