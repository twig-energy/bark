#include <utility>

#include "bark/gauge.hpp"

#include <doctest/doctest.h>

#include "bark/tags.hpp"

namespace bark
{

TEST_SUITE("Gauge")
{
    TEST_CASE("Can format gauge with no tags")
    {
        auto gauge = Gauge("metric", 42.0);
        CHECK_EQ("metric:42|g", gauge.serialize(no_tags));
    }

    TEST_CASE("Can format gauge with tags")
    {
        auto tags = Tags::from_list({"tag1:hello", "tag2:world"});

        auto gauge = Gauge("metric", 42.0).with(std::move(tags));
        CHECK_EQ("metric:42|g|#tag1:hello,tag2:world", gauge.serialize(no_tags));
    }

    TEST_CASE("Can format gauge with tags and global_tags")
    {
        auto tags = Tags::from_pairs({{"tag1", "hello"}, {"tag2", "world"}});
        auto global_tags = Tags::from_pairs({{"language", "cpp"}, {"foo", "bar"}});

        auto gauge = Gauge("metric", 1.654).with(tags);
        CHECK_EQ("metric:1.654|g|#tag1:hello,tag2:world,language:cpp,foo:bar", gauge.serialize(global_tags));
    }

    TEST_CASE("Can format gauge with global_tags")
    {
        auto global_tags = Tags::from_pairs({{"language", "cpp"}, {"foo", "bar"}});

        auto gauge = Gauge("metric", 1.777);
        CHECK_EQ("metric:1.777|g|#language:cpp,foo:bar", gauge.serialize(global_tags));
    }
}

}  // namespace bark
