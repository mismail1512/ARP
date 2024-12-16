add_library(dynamics SHARED src/dynamics.cpp)

target_include_directories(dynamics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_link_libraries(dynamics PRIVATE data_formats)