#pragma once
#include "common/marketevent.hpp"
#include "common/memorymanager.hpp"
#include "lob/limit.hpp"
#include "lob/order.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>


class Book {
    private:
        static constexpr std::size_t ORDER_POOL_CAPACITY = 2 * 1024 * 1024;
        static constexpr std::size_t LIMIT_POOL_CAPACITY = 1 * 1024 * 1024;
        
        MemoryManager<Order> orderPool;
        MemoryManager<Limit> limitPool;

        std::unordered_map<uint64_t, Order*> orderMap;

        std::vector<Limit*> ladder;
        uint64_t basePrice;
        uint64_t tickSize;

        std::size_t bestBidIdx;
        std::size_t bestAskIdx;

        std::size_t getPriceIndex(uint64_t price);

    public:
        Book() : orderPool(ORDER_POOL_CAPACITY), limitPool(LIMIT_POOL_CAPACITY), ladder(100000, nullptr), basePrice(0), tickSize(10000000), bestBidIdx(0), bestAskIdx(ladder.size()) {};

        bool addOrder(const MarketEvent& event);
        bool cancelOrder(const MarketEvent& event);
        bool modifyOrder(const MarketEvent& event);
        void clearBook();
        
};