#include <utility>

#include "bark/event.hpp"

#include <doctest/doctest.h>

#include "bark/tags.hpp"

namespace bark
{

TEST_SUITE("Event")
{
    TEST_CASE("Can format event with no tags")
    {
        auto event = Event("event", "some data here");
        CHECK_EQ("_e{5,14}:event|some data here", event.serialize(no_tags));
    }

    TEST_CASE("Can format event with tags")
    {
        auto tags = Tags::from_list({"tag1:hello", "tag2:world"});

        auto event = Event("event", "a description here").with(std::move(tags));
        CHECK_EQ("_e{5,18}:event|a description here|#tag1:hello,tag2:world", event.serialize(no_tags));
    }

    TEST_CASE("Can format event with tags and alert type")
    {
        auto tags = Tags::from_list({"tag1:hello", "tag2:world"});

        auto event = Event("event", "a description here").with(std::move(tags)).with(AlertType::Error);
        CHECK_EQ("_e{5,18}:event|a description here|t:error|#tag1:hello,tag2:world", event.serialize(no_tags));
    }

    TEST_CASE("Can format event with priority")
    {
        auto event = Event("event", "a description here").with(Priority::Low);
        CHECK_EQ("_e{5,18}:event|a description here|p:low", event.serialize(no_tags));
    }

    TEST_CASE("Can format event with tags and global_tags")
    {
        auto tags = Tags::from_pairs({{"tag1", "hello"}, {"tag2", "world"}});
        auto global_tags = Tags::from_pairs({{"language", "cpp"}, {"foo", "bar"}});

        auto event = Event("event", "a description here").with(tags);
        CHECK_EQ("_e{5,18}:event|a description here|#tag1:hello,tag2:world,language:cpp,foo:bar",
                 event.serialize(global_tags));
    }

    TEST_CASE("Can format event with global_tags")
    {
        auto global_tags = Tags::from_pairs({{"language", "cpp"}, {"foo", "bar"}});

        auto event = Event("event", "a description here");
        CHECK_EQ("_e{5,18}:event|a description here|#language:cpp,foo:bar", event.serialize(global_tags));
    }
}

}  // namespace bark
