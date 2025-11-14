cmake_minimum_required(VERSION 3.16)
project(display_driver_fault_injection_tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(test_display_driver_fault
    test_display_driver_fault.c
    stubs/mock_dependencies.c
    ../../main/drivers/display_driver.c
)

target_include_directories(test_display_driver_fault PRIVATE
    stubs
    ../../main
    ../../main/ui
    ../../main/drivers
)

if(MSVC)
    target_compile_options(test_display_driver_fault PRIVATE /W4)
else()
    target_compile_options(test_display_driver_fault PRIVATE -Wall -Wextra -Werror)
endif()

add_executable(test_ui_main_fault
    test_ui_main_fault.c
    stubs/mock_dependencies.c
    ../../main/ui/ui_main.c
)

target_include_directories(test_ui_main_fault PRIVATE
    stubs
    ../../main
    ../../main/ui
)

if(MSVC)
    target_compile_options(test_ui_main_fault PRIVATE /W4)
else()
    target_compile_options(test_ui_main_fault PRIVATE -Wall -Wextra -Werror)
endif()
