cmake_minimum_required(VERSION 3.28)

# Set default build type to release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# Generate compile_commands.json by default
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS "YES" CACHE STRING "" FORCE)
endif()

project(BananaPiano)

file(GLOB_RECURSE cpp_src src/*.cpp)
add_executable(BananaPiano ${cpp_src})

if(NOT MSVC)
    target_compile_options(BananaPiano PRIVATE -Wall -pedantic -Wextra -Werror)
else()
    # Suppress the following warnings:
    #   * C4244: lossy conversion
    target_compile_options(BananaPiano PRIVATE /wd4244 /WX)
endif()

target_compile_features(BananaPiano PUBLIC cxx_std_23)
target_include_directories(BananaPiano PRIVATE src)

include(FetchContent)

fetchcontent_declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
fetchcontent_makeavailable(SFML)
target_link_libraries(BananaPiano PUBLIC SFML::Audio SFML::Graphics)

install(TARGETS BananaPiano DESTINATION bin)
install(DIRECTORY data DESTINATION bin)
