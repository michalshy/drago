add_executable(drago_tests
    core/test_window.cpp
    rhi/test_device.cpp
)

target_link_libraries(drago_tests PRIVATE engine Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(drago_tests)