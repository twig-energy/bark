#include <utility>

#include "bark/count.hpp"

#include <doctest/doctest.h>

#include "bark/sample_rate.hpp"
#include "bark/tags.hpp"

namespace bark
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
        auto tags = Tags::from_list({"tag1:hello", "tag2:world"});

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

    TEST_CASE("Can format count with tags and global_tags")
    {
        auto tags = Tags::from_pairs({{"tag1", "hello"}, {"tag2", "world"}});
        auto global_tags = Tags::from_pairs({{"language", "cpp"}, {"foo", "bar"}});

        auto count = Count("metric", 1).with(tags);
        CHECK_EQ("metric:1|c|#tag1:hello,tag2:world,language:cpp,foo:bar", count.serialize(global_tags));
    }

    TEST_CASE("Can format count with global_tags")
    {
        auto global_tags = Tags::from_pairs({{"language", "cpp"}, {"foo", "bar"}});

        auto count = Count("metric", 1);
        CHECK_EQ("metric:1|c|#language:cpp,foo:bar", count.serialize(global_tags));
    }
}

}  // namespace bark
