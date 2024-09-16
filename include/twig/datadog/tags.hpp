#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace twig::datadog
{

namespace
{

constexpr auto tags_str_length(std::span<const std::pair<std::string_view, std::string_view>> tags) -> std::size_t
{
    auto length = std::size_t {tags.size() - 1};  // commas between tags
    for (const auto& [key, value] : tags) {
        length += key.size() + value.size() + 1;  // +1 for colons between key-value-pairs
    }
    return length;
}

constexpr auto tags_str_length(std::span<const std::string_view> tags) -> std::size_t
{
    auto length = std::size_t {tags.size() - 1};  // commas between tags
    for (const auto& tag : tags) {
        length += tag.size();
    }
    return length;
}

}  // namespace

class Tags
{
    std::string _tags;

  public:
    constexpr Tags() = default;

    constexpr explicit Tags(std::span<const std::pair<std::string_view, std::string_view>> tags)
    {
        if (tags.empty()) {
            return;
        }

        this->_tags.reserve(tags_str_length(tags) + 1);

        for (auto i = std::size_t {0}; i < tags.size(); i++) {
            const auto& [key, value] = tags[i];
            this->_tags.append(key);
            this->_tags.push_back(':');
            this->_tags.append(value);
            this->_tags.push_back(',');
        }

        this->_tags.pop_back();  // Remove trailing comma.
    }

    constexpr explicit Tags(std::span<const std::string_view> tags)
    {
        if (tags.empty()) {
            return;
        }

        this->_tags.reserve(tags_str_length(tags) + 1);

        for (auto i = std::size_t {0}; i < tags.size(); i++) {
            const auto& tag = tags[i];
            this->_tags.append(tag);
            this->_tags.push_back(',');
        }

        this->_tags.pop_back();  // Remove trailing comma.
    }

    constexpr auto str() const -> const std::string&
    {
        return this->_tags;
    }
};

constexpr const auto no_tags = Tags();

}  // namespace twig::datadog
