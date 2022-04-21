cmake_minimum_required(VERSION 3.4)

project(ray-marching)

file(GLOB SOURCES src/*.cpp)

add_executable(ray-marching ${SOURCES})

add_subdirectory(SFML)
add_subdirectory(glm)

target_include_directories(ray-marching 
    PUBLIC include
    PUBLIC SFML/include
    PUBLIC glm
)
target_link_directories(ray-marching 
    PUBLIC SFML/lib
)

target_link_libraries(ray-marching sfml-graphics sfml-window sfml-system glm)
