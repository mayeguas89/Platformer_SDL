cmake_minimum_required(VERSION 3.26.4)

project(
  TurnBased_RPG
  VERSION 0.0.0 
  DESCRIPTION "A turn based rpg game" 
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE STRING "")

include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 9.1.0
)
FetchContent_MakeAvailable(fmt)

add_subdirectory(externalLibs)

add_executable(
  ${PROJECT_NAME} 
  src/main.cc
  src/animation.cc
  src/app.cc
  src/entity.cc
  src/physics.cc
  src/player.cc
  src/sdl_input_manager.cc
  src/sdl_render.cc
  src/texture_map.cc
  src/level/level.cc
  src/ui/imgui_render.cc
  src/utils/time_manager.cc
)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(
  ${PROJECT_NAME} 
  PRIVATE 
  fmt::fmt
  glm
  imgui
  nlohmann_json::nlohmann_json
  pugixml
  spdlog::spdlog
  SDL2::SDL2
  SDL2::SDL2main
  SDL2_image::SDL2_image
)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/Assets
)

