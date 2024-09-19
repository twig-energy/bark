
#include "twig/datadog/tags.hpp"

#include <doctest/doctest.h>

namespace twig::datadog
{

TEST_SUITE("Tags")
{
    TEST_CASE("Can serialize empty tags")
    {
        auto tags = Tags {};
        CHECK_EQ("", tags.str());

        tags = Tags::from_list({});
        CHECK_EQ("", tags.str());

        tags = Tags::from_pairs({});
        CHECK_EQ("", tags.str());
    }

    TEST_CASE("Can serialize single tag")
    {
        auto tags = Tags::from_list({"tag:value"});
        CHECK_EQ("tag:value", tags.str());

        tags = Tags::from_pairs({{"tag", "value"}});
        CHECK_EQ("tag:value", tags.str());
    }

    TEST_CASE("Can serialize two tags")
    {
        auto tags = Tags::from_list({"tag:value", "other_tag:other_value"});
        CHECK_EQ("tag:value,other_tag:other_value", tags.str());

        tags = Tags::from_pairs({{"tag", "value"}, {"other_tag", "other_value"}});
        CHECK_EQ("tag:value,other_tag:other_value", tags.str());
    }
}

}  // namespace twig::datadog
