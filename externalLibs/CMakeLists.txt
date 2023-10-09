add_subdirectory(SDL2)
add_subdirectory(SDL2_Image)
add_subdirectory(glm)
add_subdirectory(nlohmann_json)
add_subdirectory(spdlog)

add_library(
  imgui 
  STATIC 
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/backends/imgui_impl_sdl2.cpp
  imgui/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(imgui 
  PUBLIC 
  imgui 
  imgui/backends
)

target_link_libraries(
  imgui 
  PRIVATE
  SDL2::SDL2 
  SDL2::SDL2main
)

add_library(
  pugixml 
  STATIC
  pugixml/src/pugixml.cpp
)

target_include_directories(
  pugixml 
  PUBLIC 
  pugixml/src
)