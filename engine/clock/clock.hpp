#pragma once
#include "common/marketevent.hpp"
#include <cassert>
#include <span>


class Clock {
    private:
        std::span<const MarketEvent> events;
        std::size_t idx = 0;
        std::uint64_t currentTime_ = 0;
        const MarketEvent& nextEvent() const;
    public:
        Clock(std::span<const MarketEvent> e): events(e) {};

        bool hasNext() const {
            return idx < events.size();
        }

        const MarketEvent& peek() const {
            // Assume caller has checked hasNext()
            return nextEvent();
        }

        const MarketEvent& next() {
            // Assume caller has checked hasNext()
            const MarketEvent& event = nextEvent();
            idx++;

            assert(currentTime_ <= event.timestamp_ns);
            
            currentTime_ = event.timestamp_ns;
            return event;
        }

        uint64_t currentTime() const {
            return currentTime_;
        }

        
};  