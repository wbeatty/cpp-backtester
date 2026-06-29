#pragma once
#include "common/marketevent.hpp"
#include <vector>
#include <memory>
#include <cstddef>
#include <iostream>
#include <sys/mman.h>

class EventSource {
    public:
        virtual ~EventSource() = default;
        virtual bool next(MarketEvent& out) = 0;
};

class csvEventSource : public EventSource {
    public:
        csvEventSource(const char* path) : inputFile(path) {load_csv();};
        ~csvEventSource() { if (file_head) munmap(file_head, file_size);}
        
        bool next(MarketEvent& out) override;
    private:
        const char* inputFile = nullptr;
        bool loaded = false;
        void load_csv();

        void *file_head;
        const char *curr_head;
        const char *file_end;
        size_t file_size;

        bool skipToNextField();
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
            if (!eventSource) {
                std::cerr << "ERROR: Event Source failed to load into DataLoader\n";
                return;
            }
            MarketEvent event;
            uint64_t i = 0;
            while (eventSource->next(event) && i < 1000) {
                eventQueue.push_back(event);
                i++;
            }
            eventSource.reset();
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