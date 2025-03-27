cmake_minimum_required(VERSION 3.10)
project(UI)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -g -fsanitize=address")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED) # Find SDL_ttf

include_directories(UI ${SDL2_INCLUDE_DIRS})
include_directories(UI ${SDL2_ttf_INCLUDE_DIRS})

add_executable(UI main.cpp sdl2_renderer_backend.cpp agui.cpp shapes.cpp widgets.cpp)
target_include_directories (UI PUBLIC sdl2_renderer_backend.h agui.h shapes.h widgets.h)

target_link_libraries(UI PUBLIC SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# add_subdirectory(AGUI)