#include <array>
#include <string_view>
#include <utility>

#include "twig/datadog/histogram.hpp"

#include <doctest/doctest.h>
#include <fmt/format.h>

#include "twig/datadog/histogram_fmt.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

TEST_SUITE("Histogram")
{
    TEST_CASE("Can format histogram with no tags")
    {
        auto histogram = Histogram("metric", 1);
        CHECK_EQ("metric:1|h", fmt::format("{}", histogram));
    }

    TEST_CASE("Can format histogram with tags")
    {
        auto tags = Tags::create(std::array<std::string_view, 2> {"tag1:hello", "tag2:world"});

        auto histogram = Histogram("metric", 42.654).with_tags(std::move(tags));
        CHECK_EQ("metric:42.654|h|#tag1:hello,tag2:world", fmt::format("{}", histogram));
    }

    TEST_CASE("Can format histogram with sample_rate")
    {
        auto histogram = Histogram("metric", 55).with_sample_rate(0.2);
        CHECK_EQ("metric:55|h|@0.2", fmt::format("{}", histogram));
    }

    TEST_CASE("Can format histogram with sample_rate and tags")
    {
        auto tags = Tags::create(std::array<std::string_view, 2> {"tag1:hello", "tag2:world"});

        auto histogram = Histogram("metric", 40.1).with_sample_rate(0.2).with_tags(std::move(tags));
        CHECK_EQ("metric:40.1|h|@0.2|#tag1:hello,tag2:world", fmt::format("{}", histogram));
    }
}

}  // namespace twig::datadog
