#include "data/dataloader.hpp"
#include "common/marketevent.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iostream>


void csvEventSource::load_csv() {
    const int fd = open(inputFile, O_RDONLY);

    if (fd == -1) {
        throw std::runtime_error("Couldn't open file");
    }

    struct stat sb{};

    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error("Couldn't get file size");
    }
    if (sb.st_size == 0) {
        close(fd);
        throw std::runtime_error("File is empty");
    }

    void *raw = mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (raw == MAP_FAILED) {
        throw std::runtime_error("failed to map file to virtual memory");
    }

    file_head = static_cast<const char*>(raw);
    file_end = file_head + sb.st_size;
    madvise(raw, sb.st_size, MADV_SEQUENTIAL);

    loaded = true;
}

bool csvEventSource::next(MarketEvent& out) {\
    // Read the next line of data, turn from CSV -> MarketEvent, return in out

    // Check if CSV has already been mapped into memory
    // Call the current ptr, and then advance to the end parsing fields we need, and then update curren to start of next, return object
    if (!loaded) {
        throw std::runtime_error("File not loaded properly");
    }

    const char *p = file_head;
    while (p++ != file_end) {
        std::cout << *p << "\n";
    }

    return false;

}