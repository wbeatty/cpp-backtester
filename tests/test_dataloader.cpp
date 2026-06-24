#include "data/dataloader.hpp"

#include <iostream>
#include <memory>

#ifndef PREVIEW_CSV
#define PREVIEW_CSV "data/preview.csv"
#endif

static constexpr std::size_t EXPECTED_EVENTS = 20;

int main() {
    const char* path = PREVIEW_CSV;

    DataLoader loader(std::make_unique<csvEventSource>(path));
    loader.load();

    const std::size_t loaded = loader.size();
    std::cout << "[test_dataloader] path=" << path
              << " loaded=" << loaded
              << " expected=" << EXPECTED_EVENTS << "\n";

    if (loaded != EXPECTED_EVENTS) {
        std::cerr << "[test_dataloader] FAILED: parsed event count mismatch\n";
        return 1;
    }

    std::cout << "[test_dataloader] PASSED\n";
    return 0;
}
