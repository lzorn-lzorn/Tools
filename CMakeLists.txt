cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Moonlight)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS 
    src/*.cpp
    src/*.c 
    include/*.h 
    include/*.hpp
    )

add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC include/)
