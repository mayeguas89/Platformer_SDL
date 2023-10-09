#include "imgui_render.h"

#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_internal.h"
#include "physics.h"
#include "utils/singleton.h"

void ImGuiRender::Init(SDL_Window* window, SDL_Renderer* renderer)
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);
}

void ImGuiRender::StartFrame()
{
  // Start the Dear ImGui frame
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}

void ImGuiRender::Render()
{
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  {
    ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.
    ImGui::ColorEdit3("clear color", (float*)&clearColor_); // Edit 3 floats representing a color
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
  }
  {
    // Entities Window
    EntitiesWindow();
  }
  {
    // Physics window
    PhysicsWindow();
  }

  // Rendering
  ImGui::Render();
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiRender::End()
{
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiRender::SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  ImU32 out;
  out = ((ImU32)IM_F32_TO_INT8_SAT(r)) << IM_COL32_R_SHIFT;
  out |= ((ImU32)IM_F32_TO_INT8_SAT(g)) << IM_COL32_G_SHIFT;
  out |= ((ImU32)IM_F32_TO_INT8_SAT(b)) << IM_COL32_B_SHIFT;
  out |= ((ImU32)IM_F32_TO_INT8_SAT(a)) << IM_COL32_A_SHIFT;
  clearColor_ = ImGui::ColorConvertU32ToFloat4(out);
}

void ImGuiRender::RegisterEntity(const std::string& name, std::shared_ptr<Entity> entity)
{
  entities_.insert({name, entity});
}

SDL_Color ImGuiRender::GetClearColor()
{
  ImU32 color{ImGui::ColorConvertFloat4ToU32(clearColor_)};
  return {(color >> IM_COL32_R_SHIFT) & 0xFF,
          (color >> IM_COL32_G_SHIFT) & 0xFF,
          (color >> IM_COL32_B_SHIFT) & 0xFF,
          (color >> IM_COL32_A_SHIFT) & 0xFF};
}

void ImGuiRender::EntitiesWindow()
{
  ImGui::Begin("Entities");
  {
    for (auto it = entities_.begin(); it != entities_.end(); it++)
    {
      auto& [name, entity] = *it;
      if (ImGui::TreeNode(name.c_str()))
      {
        auto maxSpeed = entity->GetMaxSpeed();
        float fMaxSpeed[2] = {maxSpeed.x, maxSpeed.y};
        auto currentSpeed = entity->GetCurrentSpeed();
        float fcurrentSpeed[2] = {currentSpeed.x, currentSpeed.y};
        auto position = entity->GetPosition();
        float fPosition[2] = {position.x, position.y};

        ImGui::InputFloat2("Max Speed", &fMaxSpeed[0]);
        ImGui::InputFloat2("Current Speed", &fcurrentSpeed[0]);
        ImGui::InputFloat2("Position", &fPosition[0]);

        entity->SetMaxSpeed({fMaxSpeed[0], fMaxSpeed[1]});
        entity->SetCurrentSpeed({fcurrentSpeed[0], fcurrentSpeed[1]});
        entity->SetPosition({fPosition[0], fPosition[1], 0.f, 0.f});

        ImGui::TreePop();
      }
    }
    ImGui::End();
  }
}

void ImGuiRender::PhysicsWindow()
{
  static auto& TheGravity = Gravity::Instance();
  static auto& ThePhysics = Physics::Instance();
  ImGui::Begin("Physics");
  {
    if (ImGui::TreeNode("Physics"))
    {
      bool viewDebugData = ThePhysics.viewDebugData;
      ImGui::Checkbox("View Debug Data", &viewDebugData);
      ThePhysics.viewDebugData = viewDebugData;
      ImGui::TreePop();
    }

    if (ImGui::TreeNode("Gravity"))
    {
      bool gravity = ThePhysics.gravity;
      float multiplier = TheGravity.multiplier;
      float initialGravity = TheGravity.initialGravity;
      float gravityAcceleration = TheGravity.gravityAcceleration;

      ImGui::Checkbox("Set Gravity", &gravity);
      ImGui::InputFloat("Gravity Multiplier", &multiplier);
      ImGui::InputFloat("Initial Gravity", &initialGravity);
      ImGui::InputFloat("Gravity", &gravityAcceleration);

      TheGravity.multiplier = multiplier;
      TheGravity.initialGravity = initialGravity;
      TheGravity.gravityAcceleration = gravityAcceleration;
      ThePhysics.gravity = gravity;
      ImGui::TreePop();
    }
    ImGui::End();
  }
}
