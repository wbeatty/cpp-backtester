#include "data/dataloader.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

static constexpr std::size_t EXPECTED_EVENTS = 1e7;

int main() {
    std::string datafile = "data/XNAS-20260626-CVWMCXCAKC/xnas-itch-20260525-20260624.mbo.SPY.csv";
    const char* path = datafile.c_str();

    DataLoader loader(std::make_unique<csvEventSource>(path));
    loader.load();
    
    assert(loader.size() == EXPECTED_EVENTS);

    std::cout << "[test_dataloader] PASSED\n";
    return 0;
}
