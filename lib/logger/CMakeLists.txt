add_library(logger SHARED src/logger.cpp)

target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_link_libraries(logger PRIVATE data_formats)