add_library(engine
    clock/clock.cpp
)

target_include_directories(engine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(engine PUBLIC core)