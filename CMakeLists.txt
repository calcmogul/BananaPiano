cmake_minimum_required(VERSION 3.28)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(BananaPiano)

include(FetchContent)

file(GLOB_RECURSE cpp_src src/*.cpp)
add_executable(BananaPiano ${cpp_src})

if(NOT MSVC)
    target_compile_options(BananaPiano PRIVATE -Wall -Wextra -pedantic)
else()
    # Suppress the following warnings:
    #   * C4244: lossy conversion
    target_compile_options(BananaPiano PRIVATE /wd4244)
endif()
set_property(TARGET BananaPiano PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_compile_features(BananaPiano PUBLIC cxx_std_23)
target_include_directories(BananaPiano PRIVATE src)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)
target_link_libraries(BananaPiano PUBLIC SFML::Audio SFML::Graphics)

install(TARGETS BananaPiano DESTINATION bin)
install(DIRECTORY data DESTINATION bin)
