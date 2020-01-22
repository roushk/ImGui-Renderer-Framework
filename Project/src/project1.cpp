#include <pch.hpp>
#include "project1.hpp"

#include "imgui_distance.hpp"
#include "imgui_disabled.hpp"
#include "imgui_control_point.hpp"

Project1::Project1()
{
  reset();

  //upper left to lower right
  ImGui::SetViewRect({ -0.2f, 3.4f }, { 1.2, -3.4f });
}

void Project1::draw()
{
  // Basic colors that work well with the background
  const ImU32 boxColorPacked = ImGui::ColorConvertFloat4ToU32(colorSoftLightGray);
  const ImU32 circleColorPacked = ImGui::ColorConvertFloat4ToU32(colorSoftBlue);
  const ImU32 circleColorHighlightedPacked = ImGui::ColorConvertFloat4ToU32(colorSoftWhiteBlue);

  if (drawBox)
  {
    ImGui::RenderRect({ 1.f, 1.f }, { 9.f, 9.f }, boxColorPacked, boxRounding, ImDrawCornerFlags_All, boxThickness);
  }


  ImGui::ControlPoints(controlPoints, circleRadius, circleColorPacked, circleColorHighlightedPacked, ImGuiControlPointFlags_FixX);
}

void Project1::draw_editors()
{
  static ImVec2 windowSize; // Default initializes to { 0, 0 }

  // Only show editor window if any editor buttons are active
  if (toggleDrawBox || toggleDrawCircle || showMousePosition)
  {
    // Create Editor window
    ImGui::SetNextWindowPos({ 5.f, Application::WindowHeight() - windowSize.y - 5.f });
    ImGui::SetNextWindowBgAlpha(0.2f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    if (ImGui::Begin("##Example_Editor", nullptr,
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize))
    {
      if (toggleDrawBox)
      {
        ImGui::SliderInt("Degree", &degree, 1, 50);

      }

      if (toggleDrawCircle)
      {

      }

      if (showMousePosition)
      {
        const auto mousePos = ImGui::GetMousePos();
        ImGui::Text(fmt::format("Mouse Coordinates: {0}, {1}", mousePos.x, mousePos.y).c_str());
      }

      // Get window size for next drawing
      windowSize = ImGui::GetWindowSize(); // A little hacky, but it works
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
  }
}

void Project1::draw_menus()
{
  // Create drop-down menu button
  if (ImGui::BeginMenu("Project Example Options"))
  {
    if (ImGui::MenuItem("Degree Toggler", nullptr, toggleDrawBox))
    {
      toggleDrawBox ^= 1;
    }


    // Make sure to end the menu
    ImGui::EndMenu();
  }
  
  // Add more ImGui::BeginMenu(...) for additional menus
}

void Project1::reset()
{
  drawBox = true;
  toggleDrawBox = true;
  drawCircle = true;
  toggleDrawCircle = true;
  circleRadius = 15.f;
  showMousePosition = true;
}

std::string Project1::name()
{
  return "Project Example";
}
