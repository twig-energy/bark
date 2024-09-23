#pragma once
/**
 * Single header for including all the necessary headers for the Datadog client.
 * Consider including the individual headers instead.
 *
 */

// NOLINTBEGIN(misc-include-cleaner)
#include "bark/asio_client.hpp"
#include "bark/client.hpp"
#include "bark/count.hpp"
#include "bark/datagram.hpp"
#include "bark/event.hpp"
#include "bark/gauge.hpp"
#include "bark/histogram.hpp"
#include "bark/i_datadog_client.hpp"
#include "bark/mpmc_client.hpp"
#include "bark/noop_client.hpp"
#include "bark/number_of_io_threads.hpp"
#include "bark/sample_rate.hpp"
#include "bark/spsc_client.hpp"
#include "bark/tags.hpp"
#include "bark/udp_client.hpp"
// NOLINTEND(misc-include-cleaner)
