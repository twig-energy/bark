#pragma once

#include "bark/feature_detection.hpp"

#if BARK_GCC_VERSION > 0
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wnull-dereference"
#endif

#include <asio/io_context.hpp>

#if BARK_GCC_VERSION > 0
#    pragma GCC diagnostic pop
#endif
