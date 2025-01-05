add_library(params_manager SHARED src/params_manager.cpp)


target_include_directories(params_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_link_libraries(params_manager PRIVATE data_formats yaml-cpp)