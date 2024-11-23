#pragma once

#include <concepts>
#include <utility>

#include "bark/datagram.hpp"

namespace bark
{

template<typename T>
concept sync_datagram_transport = requires(T t) {
    {
        t.send(std::declval<Datagram&&>())
    } -> std::same_as<bool>;
};

template<typename T>
concept async_datagram_transport = requires(T t) {
    {
        t.send_async(std::declval<Datagram&&>())
    } -> std::same_as<void>;
};

template<typename T>
concept datagram_transport = sync_datagram_transport<T> || async_datagram_transport<T>;

}  // namespace bark
