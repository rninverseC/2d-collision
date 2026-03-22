cmake_minimum_required(VERSION 3.20)

project(collision LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE AND EXISTS "/opt/homebrew/lib/cmake")
    list(PREPEND CMAKE_PREFIX_PATH "/opt/homebrew")
endif()

find_package(glfw3 CONFIG REQUIRED)
find_path(GLM_INCLUDE_DIR glm/glm.hpp
    HINTS
        /opt/homebrew/include
        /usr/local/include
    REQUIRED
)

add_executable(collision
    src/collision.cpp
    src/main.cpp
    src/physics.cpp
)

target_include_directories(collision PRIVATE
    ${GLM_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(collision PRIVATE glfw)

if(APPLE)
    target_link_libraries(collision PRIVATE "-framework OpenGL")
    target_compile_options(collision PRIVATE -Wno-deprecated-declarations)
endif()

add_custom_target(run
    COMMAND $<TARGET_FILE:collision>
    DEPENDS collision
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
)
