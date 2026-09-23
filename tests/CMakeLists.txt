FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.16.0 # or a later release
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(Catch2)

set(TEST_SRC ${CMAKE_CURRENT_LIST_DIR}/src/config_parser_test.cpp)

add_executable(eruptor_tests ${TEST_SRC})
target_link_libraries(eruptor_tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain eruptor_resource)
target_compile_features(eruptor_tests PRIVATE cxx_std_23)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(eruptor_tests)
