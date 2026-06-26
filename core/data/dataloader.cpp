#include "data/dataloader.hpp"
#include "common/marketevent.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <charconv>
#include <iostream>

bool csvEventSource::parseSide(uint8_t &side) {
    // A=Ask (or sell agressor in trade), B=Bid (or buy aggressor in trade), N=No side specified
    switch (*file_head) {
        case 'A':
            side = 1;
            break;
        case 'B':
            side = 2;
            break;
        case 'N':
            side = 3;
            break;
        default:
            std::cerr << "Invalid side: " << *file_head << '\n';
            file_head += 2;
            return false;
    }
    file_head += 2;
    return true;
}

bool csvEventSource::parseAction(uint8_t &event_type) {
    // A=Add, C=Cancel, M=Modify, R=Clear book, T=Trade, F=Fill, N=None
    switch (*file_head) {
        case 'A':
            event_type = 1;
            break;
        case 'C':
            event_type = 2;
            break;
        case 'M':
            event_type = 3;
            break;
        case 'R':
            event_type = 4;
            break;
        case 'T':
            event_type = 5;
            break;
        case 'F':
            event_type = 6;
            break;
        case 'N':
            event_type = 7;
            break;
        default:
            std::cerr << "Invalid Event Type: " << *file_head << '\n';
            file_head += 2;
            return false;
    }
    file_head += 2;
    return true;
}

bool csvEventSource::skipToNextField() {
    while (file_head < file_end && *file_head != ',' && *file_head != '\n' && *file_head != '\r') ++file_head;
    if (file_head < file_end && *file_head == ',') ++file_head;
    return file_head != file_end;
}

bool csvEventSource::parseValue(auto &val) {
    while (file_head < file_end && (*file_head == ' ' || *file_head == '\t')) ++file_head;
    auto [ptr, ec] = std::from_chars(file_head, file_end, val);
    if (ec != std::errc()) return false;
    ptr++;
    // File head now looking at start of next field (post comma)
    file_head = ptr;
    return true;
}

void csvEventSource::load_csv() {
    const int fd = open(inputFile, O_RDONLY);

    if (fd == -1) {
        throw std::runtime_error("Error: Couldn't open file");
    }

    struct stat sb{};

    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error("Error: Couldn't get file size");
    }
    if (sb.st_size == 0) {
        close(fd);
        throw std::runtime_error("Error: File is empty");
    }

    void *raw = mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (raw == MAP_FAILED) {
        throw std::runtime_error("Error: Failed to map file to virtual memory");
    }

    file_head = static_cast<const char*>(raw);
    file_end = file_head + sb.st_size;
    madvise(raw, sb.st_size, MADV_SEQUENTIAL);

    // Skip headers
    if (*file_head != '1' || *file_head != '2') {
        while (file_head != file_end && *file_head != '\n') ++file_head;
        if (file_head == file_end) {
            throw std::runtime_error("Error: No data found in file loading, only headers");
        }
        file_head++;
    }

    loaded = true;
}

bool csvEventSource::next(MarketEvent& out) {\
    // Read the next line of data, turn from CSV -> MarketEvent, return in out

    // Check if CSV has already been mapped into memory
    // Call the current ptr, and then advance to the end parsing fields we need, and then update curren to start of next, return object
    if (!loaded) {
        throw std::runtime_error("File not loaded properly");
    }

    if (file_head == file_end) return false;

    // Skip ts_recv (not needed in this implementation)
    skipToNextField();

    // Parse ts_event
    uint64_t timestamp_ns;
    if (!parseValue(timestamp_ns)) {
        std::cerr << "PARSING ERROR: Failed to parse timestamp\n";
    }

    // Skip rtype, publisher_id
    skipToNextField();
    skipToNextField();
    
    // Parse instrument_id
    uint32_t asset_id;
    if (!parseValue(asset_id)) {
        std::cerr << "PARSING ERROR: Failed to parse asset_id\n";
    }
    
    // Parse action
    uint8_t event_type;
    if (!parseAction(event_type)) {
        std::cerr << "PARSING ERROR: Failed to parse event_type\n";
    }

    // Parse side
    uint8_t side;
    if (!parseSide(side)) {
        std::cerr << "PARSING ERROR: Failed to parse side\n";
    }

    // Parse price
    uint64_t price;
    if (!parseValue(price)) {
        std::cerr << "PARSING ERROR: Failed to parse price\n";
    }

    // Parse size
    uint32_t size;
    if (!parseValue(size)) {
        std::cerr << "PARSING ERROR: Failed to parse size\n";
    }

    // Skip channel_id
    skipToNextField();

    // Parse order_id
    uint64_t order_id;
    if (!parseValue(order_id)) {
        std::cerr << "PARSING ERROR: Failed to parse order_id\n";
    }

    while (file_head < file_end && *file_head != '\n') ++file_head;
    if (file_head < file_end) ++file_head;

    out = MarketEvent(timestamp_ns = timestamp_ns, order_id = order_id, price = price, size = size, asset_id = asset_id, side = side, event_type = event_type);
    return true;
}