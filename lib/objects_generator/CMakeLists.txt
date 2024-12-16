add_library(objects_generator SHARED src/objects_generator.cpp)

target_include_directories(objects_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_link_libraries(objects_generator PRIVATE data_formats)