#pragma once
/**
 * Single header for including all the necessary headers for the Datadog client.
 * Consider including the individual headers instead.
 *
 */

// NOLINTBEGIN(misc-include-cleaner)
#include "twig/datadog/client.hpp"
#include "twig/datadog/count.hpp"
#include "twig/datadog/datagram.hpp"
#include "twig/datadog/event.hpp"
#include "twig/datadog/gauge.hpp"
#include "twig/datadog/histogram.hpp"
#include "twig/datadog/i_datadog_client.hpp"
#include "twig/datadog/noop_client.hpp"
#include "twig/datadog/sample_rate.hpp"
#include "twig/datadog/spsc_client.hpp"
#include "twig/datadog/tags.hpp"
#include "twig/datadog/udp_client.hpp"
// NOLINTEND(misc-include-cleaner)
