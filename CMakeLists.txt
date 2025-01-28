# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.15...3.31)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(
  DroneSim
  VERSION 1.0
  LANGUAGES CXX)

# If you set any CMAKE_ variables, that can go here.
# (But usually don't do this, except maybe for C++ standard)

# Find packages go here.

# You should usually split this into folders, but this is a simple example

# This is a "default" library, and will match the *** variable setting.
# Other common choices are STATIC, SHARED, and MODULE
# Including header files here helps IDEs but is not required.
# Output libname matches target name, with the usual extensions on your system
add_subdirectory(lib)




# add_executable(example example.cpp)
add_executable(window window.cpp)
add_executable(board board.cpp)
add_executable(obstacles_gen obstacles_gen.cpp)
add_executable(targets_gen targets_gen.cpp)
add_executable(dynamics_server dynamics_server.cpp)
add_executable(input input.cpp)
add_executable(watchdog watchdog.cpp)



# target_link_libraries(example PRIVATE data_formats objects_generator visualizer dynamics ncurses)
target_link_libraries(board PRIVATE data_formats objects_generator visualizer dynamics ncurses logger)
target_link_libraries(window PRIVATE data_formats visualizer ncurses logger)

target_link_libraries(obstacles_gen PRIVATE data_formats objects_generator logger)
target_link_libraries(targets_gen PRIVATE data_formats objects_generator logger)
target_link_libraries(dynamics_server PRIVATE params_manager data_formats objects_generator dynamics logger)
target_link_libraries(input PRIVATE data_formats logger)
target_link_libraries(watchdog PRIVATE data_formats logger)


target_include_directories(board PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(obstacles_gen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(targets_gen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(dynamics_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(input PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(watchdog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
