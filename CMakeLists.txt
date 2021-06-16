cmake_minimum_required(VERSION 3.16)

project(Linow VERSION 1.0.0)
set(CMAKE_CXX_FLAGS             "/std:c++17")

file(GLOB src src/*.cpp)

add_executable(Linow ${src})

add_subdirectory(external/glfw)
add_subdirectory(external/glew/build/cmake)

target_link_libraries(Linow glfw)
target_link_libraries(Linow glew_s)

target_include_directories(Linow PRIVATE external/glfw/include)
target_include_directories(Linow PRIVATE external/glew/include)
target_include_directories(Linow PRIVATE external/glm)
