cmake_minimum_required(VERSION 3.30)
project(KtsSignalingT VERSION 1.0.0 LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT ANDROID)
    add_compile_options(-stdlib=libc++)
    add_link_options(-stdlib=libc++)
endif()

add_subdirectory(src/kts/signaling_t)

option(KTS_BUILD_TESTS "Build Kts tests" OFF)

if(KTS_BUILD_TESTS)
    add_subdirectory(test)
endif()
