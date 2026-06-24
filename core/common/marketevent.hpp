#pragma once
#include <cstdint>

struct alignas(64) MarketEvent {
    uint64_t timestamp_ns;
    uint64_t order_id;
    uint64_t price;
    uint32_t size;
    uint32_t asset_id;
    uint8_t side; // 1 = Buy, 2 = Sell
    uint8_t event_type; // 1 = Add, 2 = Cancel, 3 = Modify, 4 = Execute 
};