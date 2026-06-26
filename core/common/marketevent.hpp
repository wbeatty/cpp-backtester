#pragma once
#include <cstdint>

struct alignas(64) MarketEvent {
    uint64_t timestamp_ns;
    uint64_t order_id;
    uint64_t price; // Multiplied by 1e9
    uint32_t size;
    uint32_t asset_id;
    uint8_t side; // 1 = Ask (sell agressor in trade), 2 = Bid (or buy agressor in trade), 3 = No side specified
    uint8_t event_type; // 1 = Add, 2 = Cancel, 3 = Modify, 4 = Clear Book, 5 = Trade, 6 = Fill, 7 = None
};