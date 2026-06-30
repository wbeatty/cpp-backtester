add_library(core
    data/dataloader.cpp
    lob/book.cpp
)

target_include_directories(core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)