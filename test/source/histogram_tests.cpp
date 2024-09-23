#include <utility>

#include "bark/histogram.hpp"

#include <doctest/doctest.h>

#include "bark/sample_rate.hpp"
#include "bark/tags.hpp"

namespace bark
{

TEST_SUITE("Histogram")
{
    TEST_CASE("Can format histogram with no tags")
    {
        auto histogram = Histogram("metric", 1);
        CHECK_EQ("metric:1|h", histogram.serialize(no_tags));
    }

    TEST_CASE("Can format histogram with tags")
    {
        auto tags = Tags::from_list({"tag1:hello", "tag2:world"});

        auto histogram = Histogram("metric", 42.654).with(std::move(tags));
        CHECK_EQ("metric:42.654|h|#tag1:hello,tag2:world", histogram.serialize(no_tags));
    }

    TEST_CASE("Can format histogram with sample_rate")
    {
        auto histogram = Histogram("metric", 55).with(SampleRate {0.2});
        CHECK_EQ("metric:55|h|@0.2", histogram.serialize(no_tags));
    }

    TEST_CASE("Can format histogram with sample_rate and tags")
    {
        auto tags = Tags::from_pairs({{"tag1", "hello"}, {"tag2", "world"}});

        auto histogram = Histogram("metric", 40.1).with(SampleRate {0.2}).with(std::move(tags));
        CHECK_EQ("metric:40.1|h|@0.2|#tag1:hello,tag2:world", histogram.serialize(no_tags));
    }

    TEST_CASE("Can format histogram with tags and global_tags")
    {
        auto tags = Tags::from_pairs({{"tag1", "hello"}, {"tag2", "world"}});
        auto global_tags = Tags::from_pairs({{"language", "cpp"}, {"foo", "bar"}});

        auto histogram = Histogram("metric", 1.654).with(tags);
        CHECK_EQ("metric:1.654|h|#tag1:hello,tag2:world,language:cpp,foo:bar", histogram.serialize(global_tags));
    }

    TEST_CASE("Can format histogram with global_tags")
    {
        auto global_tags = Tags::from_pairs({{"language", "cpp"}, {"foo", "bar"}});

        auto histogram = Histogram("metric", 1.777);
        CHECK_EQ("metric:1.777|h|#language:cpp,foo:bar", histogram.serialize(global_tags));
    }
}

}  // namespace bark
