#include "clock/clock.hpp"

const MarketEvent& Clock::nextEvent() const{
    // eventually will compare with latency queue to determine next event
    return events[idx];
}

