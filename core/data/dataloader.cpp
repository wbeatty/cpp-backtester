#include "data/dataloader.hpp"
#include "common/marketevent.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <charconv>

bool csvEventSource::skipToNextField() {
    while (file_head < file_end && *file_head != ',' && *file_head != '\n' && *file_head != '\r') ++file_head;
    if (file_head < file_end && *file_head == ',') ++file_head;
    return file_head != file_end;
}

bool csvEventSource::parseValue(auto &val) {
    while (file_head < file_end && (*file_head == ' ' || *file_head == '\t')) ++file_head;
    auto [ptr, ec] = std::from_chars(file_head, file_end, val);
    if (ec != std::errc()) return false;
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
    if (!parseISO(timestamp_ns)) {
        // throw error
    }

    // Skip rtype, publisher_id
    skipToNextField();
    skipToNextField();
    
    // Parse instrument_id
    uint32_t asset_id;
    if (!parseValue(asset_id)) {
        // throw error
    }

    // Parse action
    uint8_t event_type;
    if (!parseAction(event_type)) {
        // throw error
    }

    // Parse side
    uint8_t side;
    if (!parseSide(side)) {
        // throw error
    }

    // Parse price
    // Update to multiply first by 1e9 then store
    uint64_t price;
    if (!parseValue(price)) {
        // throw error
    }

    // Parse size
    uint32_t size;
    if (!parseValue(size)) {
        // throw error
    }

    // Skip channel_id
    skipToNextField();

    // Parse order_id
    uint64_t order_id;
    if (!parseValue(order_id)) {
        // throw error
    }

    while (file_head < file_end && *file_head != '\n') ++file_head;
    if (file_head < file_end) ++file_head;

    return true;

}