add_library(engine
    engine.cpp
)

target_include_directories(engine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)