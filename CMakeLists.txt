cmake_minimum_required(VERSION 3.16)
project(robot_sim CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(robot_sim 
    src/shader.cpp 
    glad/src/glad.c 
)

target_include_directories(robot_sim PRIVATE glad/include) 
target_include_directories(robot_sim PRIVATE include)
target_link_libraries(robot_sim PRIVATE glfw OpenGL::GL)