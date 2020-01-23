#include <pch.hpp>
#include "project1.hpp"

#include "imgui_distance.hpp"
#include "imgui_disabled.hpp"
#include "imgui_control_point.hpp"

Project1::Project1()
{
  pyramid.resize(maxDegree + 1);

  for (unsigned x = 0; x < maxDegree + 1; ++x)
  {

    pyramid[x].resize(maxDegree + 1);

    for (unsigned y = 0; y < maxDegree + 1; ++y)
    {
      pyramid[x][y] = XChooseYCache(x, y);

    }
  }
 
  reset();

  //upper left to lower right
  ImGui::SetViewRect({ -0.2f, 3.4f }, { 1.2f, -3.4f });
}

void Project1::draw()
{
  // Basic colors that work well with the background
  const ImU32 boxColorPacked = ImGui::ColorConvertFloat4ToU32(colorSoftLightGray);
  const ImU32 circleColorPacked = ImGui::ColorConvertFloat4ToU32(colorSoftBlue);
  const ImU32 circleColorHighlightedPacked = ImGui::ColorConvertFloat4ToU32(colorSoftWhiteBlue);

  if (drawBox)
  {
    ImGui::RenderRect({ 0.0f, 3.0f }, { 1.0f, -3.0f }, boxColorPacked, boxRounding, ImDrawCornerFlags_All, boxThickness);
    
    ImGui::RenderLine({ 0.0f, 2.0f }, { 1.0f, 2.0f }, boxColorPacked, lineThickness);
    ImGui::RenderLine({ 0.0f, 1.0f }, { 1.0f, 1.0f }, boxColorPacked, lineThickness);
    ImGui::RenderLine({ 0.0f, 0.0f }, { 1.0f, 0.0f }, boxColorPacked, lineThickness * 3.0f);
    ImGui::RenderLine({ 0.0f, -1.0f }, { 1.0f, -1.0f }, boxColorPacked, lineThickness);
    ImGui::RenderLine({ 0.0f, -2.0f }, { 1.0f, -2.0f }, boxColorPacked, lineThickness);


    for (unsigned i = 1; i < 10; ++i)
    {
      ImGui::RenderLine({ 0.1f * i, 3.0f }, { 0.1f * i, -3.0f }, boxColorPacked, lineThickness);
    }
  }

  if (ImGui::ControlPoints(controlPoints, circleRadius, circleColorPacked, circleColorHighlightedPacked, ImGuiControlPointFlags_FixX))
  {
    CalculatePoints();
  }
  DrawFunction();

}

void Project1::draw_editors()
{
  int oldDegree = degree;
  static ImVec2 windowSize; // Default initializes to { 0, 0 }

  // Only show editor window if any editor buttons are active
  if (toggleDrawCircle)
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
      
      ImGui::SliderInt("Degree", &degree, 1, maxDegree);

      // Get window size for next drawing
      windowSize = ImGui::GetWindowSize(); // A little hacky, but it works
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
  }

  if (oldDegree != degree)
  {
    ResizeControlPoints();
  }
}

void Project1::draw_menus()
{
  // Create drop-down menu button
  if (ImGui::BeginMenu("Project 1 Options"))
  {
    if (ImGui::MenuItem("NLI", nullptr, usingNLI))
    {
      usingNLI = true;
    }
    if (ImGui::MenuItem("BB-Form", nullptr, !usingNLI))
    {
      usingNLI = false;
    }
    // Make sure to end the menu
    ImGui::EndMenu();
  }
  
  // Add more ImGui::BeginMenu(...) for additional menus
}

void Project1::reset()
{
  points.resize(quality);
  ResizeControlPoints();

  drawBox = true;
  drawCircle = true;
  toggleDrawCircle = true;
  circleRadius = 15.f;
}

std::string Project1::name()
{
  return "Project 1";
}

void Project1::ResizeControlPoints()
{
  controlPoints.resize(degree + 1);

  //set x values
  for (unsigned i = 0; i < degree + 1; ++i)
  {
    controlPoints[i].x = (i / (float)degree);
  }
  CalculatePoints();
}

void Project1::DrawFunction()
{
  for (unsigned i = 1; i < quality; ++i)
  {
    ImGui::RenderLine(points[i - 1], points[i], ImGui::ColorConvertFloat4ToU32(colorSoftLightGreen));
  }
}


//NLI recursive func
float NLIFunc(std::vector<ControlPoint>& points, float t)
{
  //if 1 point
  if (points.size() == 1)
  {
    return points[0].y;
  }


  //if 2+ points
  if (points.size() >= 2)
  {
    std::vector<ControlPoint> lhs;
    std::vector<ControlPoint> rhs;


    lhs = std::vector(points.begin(), points.end() - 1);
    rhs = std::vector(points.begin() + 1, points.end());

    return (1 - t) * NLIFunc(lhs, t) + t * NLIFunc(rhs, t);
  }
  
  return 0;

}

//(d,i) = (d-1,i-1) + (d-1, i)
//(d,0) = (d,d) = 1
//(d,y < 0) = (d, d + 1) = (0,d) = 0


int XChooseYCache(int x, int y)
{

  //base case
  if (x == y || y == 0)
    return 1;

  //error case
  if (y < 0 || y > x || x == 0)
  {
    return 0;
  }

  if (pyramid[x][y] != 0)
    return pyramid[x][y];

  //recursive case
  return XChooseYCache(x - 1, y - 1) + XChooseYCache(x - 1, y);
}


int XChooseY(int x, int y)
{
  return pyramid[x][y];
}



void Project1::CalculatePoints()
{

  if (usingNLI)
  {
    //NLI
    for (unsigned i = 0; i < quality; ++i)
    {
      //float t = current x value of this point on the graph (the current i / quality as float)
      float t = i / float(quality);

      //p(t) = p[a0,a1,a2,...aN](t) = (1 - t)p[a0,a1,a2,...a(N - 1)](t) + (t)p[a1,a2,...a(N)]
      float p = NLIFunc(controlPoints, t);
      points[i] = (ImVec2(t,p));

    }
  }
  else
  {
    //BB-Form
    for (unsigned i = 0; i < quality; ++i)
    {

      float t = i / float(quality);

      float p = 0;

      for (unsigned j = 0; j < controlPoints.size(); ++j)
      {
        p += controlPoints[j].y* XChooseY(degree, j)* pow((1.0f - t), (degree - j))* pow((t), (j));
      }

      points[i] = (ImVec2(t, p));

    }
  }

}


