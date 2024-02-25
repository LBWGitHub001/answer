cmake_minimum_required(VERSION 3.5)
project(answer)

# Default to C99
if (NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif ()

# Default to C++17
if (NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif ()

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif ()

add_definitions(-DFRONT_END="${PROJECT_SOURCE_DIR}/frontend/winter_homework_2024.x86_64")

include_directories(thirdparty/websocketpp-0.8.2)

find_package(ament_cmake REQUIRED)
find_package(OpenCV REQUIRED)

# find dependencies and libraries
set(AMENT_DEPENDENCIES
        rclcpp
        geometry_msgs
        sensor_msgs
        cv_bridge
)

foreach (AMENT_DEPENDENCY ${AMENT_DEPENDENCIES})
  find_package(${AMENT_DEPENDENCY} REQUIRED)
endforeach ()

add_executable(${PROJECT_NAME}_node
        src/answer.cpp
        include/answer/Terminal.h
        include/answer/Terminal.cpp
        include/answer/function.cpp
)
target_link_libraries(${PROJECT_NAME}_node ${OpenCV_LIBRARIES})
ament_target_dependencies(${PROJECT_NAME}_node ${AMENT_DEPENDENCIES})
target_include_directories(${PROJECT_NAME}_node PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

install(TARGETS ${PROJECT_NAME}_node
        DESTINATION lib/${PROJECT_NAME})

if (BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif ()

install(DIRECTORY
	launch
	DESTINATION share/${PROJECT_NAME}/
)



ament_package()
