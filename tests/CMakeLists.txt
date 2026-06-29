add_executable(test_dataloader test_dataloader.cpp)
target_link_libraries(test_dataloader PRIVATE core)

add_executable(test_clock test_clock.cpp)
target_link_libraries(test_clock PRIVATE engine)


target_compile_definitions(test_dataloader
    PRIVATE
    PREVIEW_CSV="${CMAKE_SOURCE_DIR}/data/preview.csv"
)

target_compile_definitions(test_clock
    PRIVATE
    DATAFILE="${CMAKE_SOURCE_DIR}/data/XNAS-20260626-CVWMCXCAKC/xnas-itch-20260525-20260624.mbo.SPY.csv"
)

add_test(NAME test_dataloader COMMAND test_dataloader)
add_test(NAME test_clock COMMAND test_clock)
