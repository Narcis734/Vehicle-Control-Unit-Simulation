cmake_minimum_required(VERSION 3.16)
project(vehicle_control_unit C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(vcu
    src/application/vcu.c
    src/input_sim/input_sim.c
    src/middleware/pedal.c
)
target_include_directories(vcu PUBLIC include)

if(MSVC)
    target_compile_options(vcu PRIVATE /W4)
else()
    target_compile_options(vcu PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(vcu_demo app/main.c)
target_link_libraries(vcu_demo PRIVATE vcu)

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG v2.6.1
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(unity)

    add_executable(vcu_tests tests/test_vcu.c)
    target_link_libraries(vcu_tests PRIVATE vcu unity)
    if(MSVC)
        target_compile_options(vcu_tests PRIVATE /W4)
    else()
        target_compile_options(vcu_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_test(NAME vcu_tests COMMAND vcu_tests)
endif()
