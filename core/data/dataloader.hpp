#pragma once
#include "common/marketevent.hpp"
#include <vector>
#include <memory>
#include <cstddef>

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
        bool parseAction(uint8_t event_type);
        bool parseSide(uint8_t side);
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
            while (eventSource->next(event)) {
                eventQueue.push_back(event);
            }
        }

        std::size_t size() const { return eventQueue.size(); }
        const std::vector<MarketEvent>& events() const { return eventQueue; }

        



};