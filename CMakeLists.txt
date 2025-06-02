# Toolchain z vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/Users/Tomek/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

cmake_minimum_required(VERSION 3.10)
project(SkakaniePoDachach)

set(CMAKE_CXX_STANDARD 17)

# Zależności przez vcpkg
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(SkakaniePoDachach
    src/main.cpp
    src/shader.cpp src/Shader.h
    src/player.cpp src/Player.h
    src/platform.cpp src/Platform.h
    src/Model.cpp src/Model.h # Dodane
    src/Mesh.cpp src/Mesh.h   # Dodane
    src/cube.cpp src/cube.h   # Na razie zostawiamy, później usuniemy
    src/stb_image.cpp         # Dodane, upewnij się, że masz plik .cpp z implementacją
)

target_link_libraries(SkakaniePoDachach
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm
    assimp::assimp
)