cmake_minimum_required(VERSION 3.10)
project(SolarSystem)

set(CMAKE_CXX_STANDARD 14)

cmake_policy(SET CMP0072 NEW)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

include_directories(
    ${GLEW_INCLUDE_DIRS}
    ${glfw3_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    /usr/local/include/soil2
)

add_executable(${PROJECT_NAME} SolarSystem/src/main.cpp)

target_link_libraries(
    SolarSystem
    ${GLEW_LIBRARIES}
    ${glfw3_LIBRARIES}
    glfw
    ${GLM_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    soil2
    OpenGL::GL
)
