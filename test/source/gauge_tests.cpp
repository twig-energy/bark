#include <utility>

#include "twig/datadog/gauge.hpp"

#include <doctest/doctest.h>

#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

TEST_SUITE("Gauge")
{
    TEST_CASE("Can format gauge with no tags")
    {
        auto gauge = Gauge("metric", 42.0);
        CHECK_EQ("metric:42|g", gauge.serialize());
    }

    TEST_CASE("Can format gauge with tags")
    {
        auto tags = Tags::from_tags({"tag1:hello", "tag2:world"});

        auto gauge = Gauge("metric", 42.0).with(std::move(tags));
        CHECK_EQ("metric:42|g|#tag1:hello,tag2:world", gauge.serialize());
    }
}

}  // namespace twig::datadog
