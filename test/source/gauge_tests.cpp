#include <utility>

#include "twig/datadog/gauge.hpp"

#include <doctest/doctest.h>
#include <fmt/format.h>

#include "twig/datadog/detail/fmt.hpp"
#include "twig/datadog/tags.hpp"

namespace twig::datadog
{

TEST_SUITE("Gauge")
{
    TEST_CASE("Can format gauge with no tags")
    {
        auto gauge = Gauge("metric", 42.0);
        CHECK_EQ("metric:42|g", fmt::format("{}", gauge));
    }

    TEST_CASE("Can format gauge with tags")
    {
        auto tags = Tags::from_tags({"tag1:hello", "tag2:world"});

        auto gauge = Gauge("metric", 42.0).with_tags(std::move(tags));
        CHECK_EQ("metric:42|g|#tag1:hello,tag2:world", fmt::format("{}", gauge));
    }
}

}  // namespace twig::datadog
