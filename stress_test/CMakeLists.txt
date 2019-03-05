cmake_minimum_required(VERSION 3.5)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

project(stress_test)

# dependencies
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# Make executable
set(STRESS_TEST_EXE "stress_test_exe")
add_executable(${STRESS_TEST_EXE} src/stress_test.cpp)
ament_target_dependencies(${STRESS_TEST_EXE}
  "rclcpp"
  "std_msgs"
)

ament_package()
