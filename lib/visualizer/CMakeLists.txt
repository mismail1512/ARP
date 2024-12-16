add_library(visualizer SHARED src/visualizer.cpp)

target_include_directories(visualizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_link_libraries(visualizer PRIVATE data_formats ncurses)