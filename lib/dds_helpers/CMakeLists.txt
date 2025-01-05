
# Set C++11
include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(-std=c++11)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

message(STATUS "Configuring HelloWorld publisher/subscriber example...")
file(GLOB DDS_SOURCES_CXX 
    "Generated/Obstacles/*.cxx"
    "Generated/Targets/*.cxx"
)
add_library(DDSGeneral ${DDS_SOURCES_CXX})
target_link_libraries(DDSGeneral PUBLIC fastdds fastcdr)
target_include_directories(DDSGeneral PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/Generated/Targets ${CMAKE_CURRENT_SOURCE_DIR}/Generated/Obstacles)


add_library(DDSPublisher INTERFACE)
target_link_libraries(DDSPublisher INTERFACE DDSGeneral)
target_include_directories(DDSPublisher INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(DDSSubscriber INTERFACE)
target_link_libraries(DDSSubscriber INTERFACE DDSGeneral)
target_include_directories(DDSSubscriber INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include )