#pragma once
#include "common/marketevent.hpp"
#include <vector>
#include <memory>
#include <cstddef>
#include <iostream>

class EventSource {
    public:
        virtual ~EventSource() = default;
        virtual bool next(MarketEvent& out) = 0;
};

class csvEventSource : public EventSource {
    public:
        csvEventSource(const char* path) : inputFile(path) {load_csv();};
        bool next(MarketEvent& out) override;
    private:
        const char* inputFile = nullptr;
        bool loaded = false;
        void load_csv();
        const char *file_head;
        const char *file_end;

        bool skipToNextField();
        bool parseISO(uint64_t &timestamp_ns);
        bool parseValue(auto &val);
        bool parseAction(uint8_t &event_type);
        bool parseSide(uint8_t &side);
};

class DataLoader {
    private:
        std::unique_ptr<EventSource> eventSource;
        std::vector<MarketEvent> eventQueue;
        
    public:
        explicit DataLoader(std::unique_ptr<EventSource> src) : eventSource(std::move(src)) {
            eventQueue.reserve(100000);
        };

        void load() {
            MarketEvent event;
            uint64_t i = 0;
            while (eventSource->next(event) && i < 1e7) {
                eventQueue.push_back(event);
                i++;
            }
        }

        std::size_t size() const { return eventQueue.size(); }
        const std::vector<MarketEvent>& events() const { return eventQueue; }
        
        void printTop() {
            MarketEvent& event = eventQueue[0];
            std::cout << "Timestamp: " << event.timestamp_ns << '\n';
            std::cout << "Order ID: " << event.order_id << '\n';
            std::cout << "Price: " << event.price << '\n';
            std::cout << "Size: " << event.size << '\n';
            std::cout << "Asset ID: " << event.asset_id << '\n';
            std::cout << "Side: " << +event.side << '\n';
            std::cout << "Event Type: " << +event.event_type << '\n';
        }

        



};