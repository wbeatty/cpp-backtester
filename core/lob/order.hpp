#pragma once
#include <cstdint>

struct Limit;

struct Order {
    uint64_t order_id;
    uint32_t size;
    uint8_t side; // 1 = Ask (sell agressor in trade), 2 = Bid (or buy agressor in trade), 3 = No side specified

    Order *prev;
    Order* next;

    Limit* parent;
};