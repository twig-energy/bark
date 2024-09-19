#include <utility>

#include "twig/datadog/count.hpp"

#include <doctest/doctest.h>

#include "twig/datadog/sample_rate.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

TEST_SUITE("Count")
{
    TEST_CASE("Can format count with no tags")
    {
        auto count = Count("metric", 1);
        CHECK_EQ("metric:1|c", count.serialize(no_tags));
    }

    TEST_CASE("Can format count with tags")
    {
        auto tags = Tags::from_tags({"tag1:hello", "tag2:world"});

        auto count = Count("metric", 42).with(std::move(tags));
        CHECK_EQ("metric:42|c|#tag1:hello,tag2:world", count.serialize(no_tags));
    }

    TEST_CASE("Can format count with sample_rate")
    {
        auto count = Count("metric", 55).with(SampleRate {0.2});
        CHECK_EQ("metric:55|c|@0.2", count.serialize(no_tags));
    }

    TEST_CASE("Can format count with sample_rate and tags")
    {
        auto tags = Tags::from_pairs({{"tag1", "hello"}, {"tag2", "world"}});

        auto count = Count("metric", 40).with(SampleRate {0.2}).with(std::move(tags));
        CHECK_EQ("metric:40|c|@0.2|#tag1:hello,tag2:world", count.serialize(no_tags));
    }
}

}  // namespace twig::datadog
