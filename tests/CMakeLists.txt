add_executable(test_dataloader test_dataloader.cpp)
target_link_libraries(test_dataloader PRIVATE core)

target_compile_definitions(test_dataloader
    PRIVATE
    PREVIEW_CSV="${CMAKE_SOURCE_DIR}/data/preview.csv"
)

add_test(NAME test_dataloader COMMAND test_dataloader)
