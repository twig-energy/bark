#include <array>
#include <string_view>
#include <utility>

#include "twig/datadog/count.hpp"

#include <doctest/doctest.h>
#include <fmt/format.h>

#include "twig/datadog/detail/fmt.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

TEST_SUITE("Count")
{
    TEST_CASE("Can format count with no tags")
    {
        auto count = Count("metric", 1);
        CHECK_EQ("metric:1|c", fmt::format("{}", count));
    }

    TEST_CASE("Can format count with tags")
    {
        auto tags = Tags::create(std::array<std::string_view, 2> {"tag1:hello", "tag2:world"});

        auto count = Count("metric", 42).with_tags(std::move(tags));
        CHECK_EQ("metric:42|c|#tag1:hello,tag2:world", fmt::format("{}", count));
    }

    TEST_CASE("Can format count with sample_rate")
    {
        auto count = Count("metric", 55).with_sample_rate(0.2);
        CHECK_EQ("metric:55|c|@0.2", fmt::format("{}", count));
    }

    TEST_CASE("Can format count with sample_rate and tags")
    {
        auto tags = Tags::create(std::array<std::string_view, 2> {"tag1:hello", "tag2:world"});

        auto count = Count("metric", 40).with_sample_rate(0.2).with_tags(std::move(tags));
        CHECK_EQ("metric:40|c|@0.2|#tag1:hello,tag2:world", fmt::format("{}", count));
    }
}

}  // namespace twig::datadog
