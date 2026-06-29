#include "data/dataloader.hpp"
#include "clock/clock.hpp"

#include <cassert>
#include <iostream>

int main() {

    DataLoader loader(std::make_unique<csvEventSource>(DATAFILE));
    loader.load();

    Clock c = Clock(loader.events());

    size_t ticks = 0;
    uint64_t currentTime = 0;

    while (c.hasNext()) {
        const MarketEvent& event = c.next();
        assert(currentTime <= event.timestamp_ns);
        ticks++;
        currentTime = event.timestamp_ns;
    }

    assert(ticks == loader.events().size());

    std::cout << "[test_clock] PASSED\n";
    return 0;
}
