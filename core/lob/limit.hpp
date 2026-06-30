#pragma once
#include "order.hpp"
#include <cstddef>

struct Limit {
    uint64_t price;
    
    Order* head;
    Order* tail;

    uint64_t totalVolume;
    std::size_t orderCount;

    void addOrder(Order* order) {
        if (head == nullptr) {
            head = order;
            tail = order;
        }
        else {
            tail->next = order;
            order->prev = tail;
            tail = order;
        }
        order->next = nullptr;
        order->parent = this;
        orderCount++;
        totalVolume += order->size;
    }

    void removeOrder(Order* order) {
        if (order->prev != nullptr) {
            order->prev->next = order->next;
        }
        else {
            head = order->next;
        }
        if (order->next != nullptr) {
            order->next->prev = order->prev;
        }
        else {
            tail = order->prev;
        }
        order->prev = nullptr;
        order->next = nullptr;
        orderCount--;
        totalVolume -= order->size;
    }
};