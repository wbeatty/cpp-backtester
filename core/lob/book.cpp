#include "lob/book.hpp"
#include <cassert>

std::size_t Book::getPriceIndex(uint64_t price) {
    uint64_t idx = (price - basePrice) / tickSize;
    assert(idx > 0 && idx < ladder.size());
    return idx;
}

bool Book::addOrder(const MarketEvent& event) {
    std::size_t idx = getPriceIndex(event.price);

    if (ladder[idx] == nullptr) {
        Limit* limit = limitPool.allocate();
        limit->price = event.price;
        ladder[idx] = limit;
    }

    Order* order = orderPool.allocate();
    order->order_id = event.order_id;
    order->size = event.size;
    order->side = event.side;

    ladder[idx]->addOrder(order);

    assert(!orderMap.contains(event.order_id));

    orderMap[event.order_id] = order;

    if (event.side == 2 && idx > bestBidIdx) {
        bestBidIdx = idx;
    }
    if (event.side == 1 && idx < bestAskIdx) {
        bestAskIdx = idx;
    }
    return true;
}

bool Book::cancelOrder(const MarketEvent& event) {
    if (orderMap.contains(event.order_id)) {
        Order* order = orderMap[event.order_id];
        Limit* limit = order->parent;
        limit->removeOrder(order);

        if (limit->orderCount == 0) {
            std::size_t idx = getPriceIndex(limit->price);
            ladder[idx] = nullptr;
            // Find new best bid or best ask if current (now depleted) price level was either
            if (idx == bestBidIdx) {
                while (idx > 0 && ladder[idx] == nullptr) {
                    idx--;
                }
                bestBidIdx = idx;
            }
            else if (idx == bestAskIdx) {
                while(idx < ladder.size() && ladder[idx] == nullptr) {
                    idx++;
                }
                bestAskIdx = idx;
            }
            limitPool.deallocate(limit);
        }       
        orderMap.erase(event.order_id);
        orderPool.deallocate(order);
        return true;
    }
    return false;
}

bool Book::modifyOrder(const MarketEvent& event) {
    if (orderMap.contains(event.order_id)) {
        // If just size decreased, modify in place
        // If price changed or size increased, cancel and re-add order
        Order* order = orderMap[event.order_id];
        if (order->size >= event.size && order->parent->price == event.price) {
            // size delta, difference between original and now reduced size
            uint64_t d = order->size - event.size;
            order->size -= d;
            order->parent->totalVolume -= d;
        }
        else {
            cancelOrder(event);
            addOrder(event);
        }
        return true;
    }
    return false;
}