#pragma once
/**
 * Single header for including all the necessary headers for the Datadog client.
 * Consider including the individual headers instead.
 *
 */

#include "bark/asio_client.hpp"           // IWYU pragma: export
#include "bark/client.hpp"                // IWYU pragma: export
#include "bark/count.hpp"                 // IWYU pragma: export
#include "bark/datagram.hpp"              // IWYU pragma: export
#include "bark/event.hpp"                 // IWYU pragma: export
#include "bark/gauge.hpp"                 // IWYU pragma: export
#include "bark/histogram.hpp"             // IWYU pragma: export
#include "bark/i_datadog_client.hpp"      // IWYU pragma: export
#include "bark/mpmc_client.hpp"           // IWYU pragma: export
#include "bark/noop_client.hpp"           // IWYU pragma: export
#include "bark/number_of_io_threads.hpp"  // IWYU pragma: export
#include "bark/sample_rate.hpp"           // IWYU pragma: export
#include "bark/spsc_client.hpp"           // IWYU pragma: export
#include "bark/tags.hpp"                  // IWYU pragma: export
#include "bark/udp_client.hpp"            // IWYU pragma: export
