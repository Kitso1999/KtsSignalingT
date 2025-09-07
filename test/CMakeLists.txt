include(FetchContent)

FetchContent_Declare(
        GoogleTest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG a6ce08abf746c0aaa577520d6d1f6ea2abeeb61d
)

FetchContent_MakeAvailable(GoogleTest)

add_executable(test-signaling_t test_signaling_t.cpp)

target_compile_features(test-signaling_t PRIVATE cxx_std_17)
target_link_libraries(test-signaling_t PRIVATE Kts::SignalingT gtest_main)

include(GoogleTest)
gtest_discover_tests(test-signaling_t)
