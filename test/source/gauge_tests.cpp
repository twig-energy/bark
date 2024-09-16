#include <array>
#include <string_view>

#include "twig/datadog/gauge.hpp"

#include <doctest/doctest.h>
#include <fmt/format.h>

#include "twig/datadog/gauge_fmt.hpp"

namespace twig::datadog
{

TEST_SUITE("Gauge")
{
    TEST_CASE("Can format gauge with default rate and no tags")
    {
        auto gauge = Gauge("metric", 42.0);
        CHECK_EQ("metric:42|g", fmt::format("{}", gauge));
    }

    TEST_CASE("Can format gauge with rate and no tags")
    {
        auto gauge = Gauge("metric", 42.0, 0.5);
        CHECK_EQ("metric:42|g|@0.5", fmt::format("{}", gauge));
    }

    TEST_CASE("Can format gauge with default rate and tags")
    {
        auto tags = Tags {std::array<std::string_view, 2> {"tag1:hello", "tag2:world"}};

        auto gauge = Gauge("metric", 42.0, 1.0, tags);
        CHECK_EQ("metric:42|g|#tag1:hello,tag2:world", fmt::format("{}", gauge));
    }

    TEST_CASE("Can format gauge with rate and tags")
    {
        auto tags = Tags {std::array<std::string_view, 2> {"tag1:hello", "tag2:world"}};

        auto gauge = Gauge("metric", 42.0, 0.75, tags);
        CHECK_EQ("metric:42|g|@0.75|#tag1:hello,tag2:world", fmt::format("{}", gauge));
    }
}

}  // namespace twig::datadog
