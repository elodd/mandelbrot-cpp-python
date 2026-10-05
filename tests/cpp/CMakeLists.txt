include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Unit tests for the C++ calculation
add_executable(test_mandelbrot test_mandelbrot.cpp)
target_include_directories(test_mandelbrot PRIVATE ${PROJECT_SOURCE_DIR}/cpp)
target_link_libraries(test_mandelbrot PRIVATE GTest::gtest_main Threads::Threads)
include(GoogleTest)
gtest_discover_tests(test_mandelbrot)

# End-to-end: run the program, C++ computes and Python saves the image
add_test(NAME end_to_end
         COMMAND mandelbrot ${CMAKE_CURRENT_BINARY_DIR}/end_to_end.png
         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_tests_properties(end_to_end PROPERTIES
  ENVIRONMENT "MPLBACKEND=Agg"
  PASS_REGULAR_EXPRESSION "Python: saved")
