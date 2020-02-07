#include <pch.hpp>

#include "project2.hpp"
#include "project1.hpp"


#include "imgui_distance.hpp"
#include "imgui_disabled.hpp"
#include "imgui_control_point.hpp"

#include <sstream>
#include <iomanip> 


Project2::Project2()
{
  //upper left to lower right
  ImGui::SetViewRect({ -0.2f, 3.4f }, { 1.2f, -3.4f });
  reset();
}


void Project2::reset()
{
  BernsteinPoly.SetQuality(maxDegree);
  points.resize(quality);
  ResizeControlPoints();

  drawBox = true;
  drawCircle = true;
  toggleDrawCircle = true;
  circleRadius = 15.f;
}

std::string Project2::name()
{
  return "Project 2";
}

void Project2::ResizeControlPoints()
{
  controlPoints.resize(degree + 1);

  //set x values
  for (unsigned i = 0; i < degree + 1; ++i)
  {
    controlPoints[i].x = (i / (float)degree);
  }
  CalculatePoints();
}

void Project2::DrawFunction()
{
  for (unsigned i = 1; i < quality; ++i)
  {
    ImGui::RenderLine(points[i - 1], points[i], ImGui::ColorConvertFloat4ToU32(colorSoftLightGreen));
  }
}


void Project2::draw()
{
  // Basic colors that work well with the background
  const ImU32 boxColorPacked = ImGui::ColorConvertFloat4ToU32(colorSoftLightGray);
  const ImU32 circleColorPacked = ImGui::ColorConvertFloat4ToU32(colorSoftBlue);
  const ImU32 circleColorHighlightedPacked = ImGui::ColorConvertFloat4ToU32(colorSoftWhiteBlue);
  const ImU32 white = ImGui::ColorConvertFloat4ToU32(colorWhite);
  const ImU32 blue = ImGui::ColorConvertFloat4ToU32(colorSoftBlue);


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

  if (ImGui::ControlPoints(controlPoints, circleRadius, circleColorPacked, circleColorHighlightedPacked, ImGuiControlPointFlags_ClampXY | ImGuiControlPointFlags_AddAndRemoveWithMouse) && controlPoints.size() > 0)
  {
    CalculatePoints();
  }


  for (unsigned i = 0; i < controlPoints.size(); ++i)
  {
    
    std::stringstream pos;
    pos << std::setfill(' ') << std::setw(4) << std::setprecision(2) << float(controlPoints[i].x) << " ," << controlPoints[i].y;
    //std::string pos2 = pos.str();
    ImGui::RenderText(controlPoints[i].ToImVec2(-0.061f,-0.2f), white, pos.str().c_str());

    //Display which number point this is
    //ImGui::RenderText(controlPoints[i].ToImVec2(-0.01f, 0.1f), white, std::to_string(i).c_str());
    
  }
  
  for (int i = 0; i < int(controlPoints.size()) - 1; ++i)
  { 
    ImGui::RenderLine(controlPoints[i].ToImVec2(), controlPoints[i + 1].ToImVec2(), white, lineThickness);
  }

  RecalculateShellNLI();


  for (int i = 1; i < controlPointCopy2D.size(); ++i)
  {
    for (int j = 0; j < controlPointCopy2D[i].size() - 1; ++j)
    {
      ImGui::RenderLine(controlPointCopy2D[i][j].ToImVec2(), controlPointCopy2D[i][j + 1].ToImVec2(), blue, lineThickness);
    }
  }

  DrawFunction();
  
}

void Project2::draw_editors()
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

    if (ImGui::Begin("##Example_Editor2", nullptr,
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize))
    {
      
      ImGui::SliderFloat("T value for NLI", &tValueNLI,0,1);


      // Get window size for next drawing
      windowSize = ImGui::GetWindowSize(); // A little hacky, but it works
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
  }

  if (oldDegree != degree)
  {

    //Bounds check degree
    degree = std::max<int>(degree, 1);
    degree = std::min<int>(degree, maxDegree);
    ResizeControlPoints();
  }
}

void Project2::draw_menus()
{
  bool oldNLI = usingNLI;
  // Create drop-down menu button
  if (ImGui::BeginMenu("Project 2 Options"))
  {
    if (ImGui::MenuItem("NLI", nullptr, usingNLI))
    {
      currentMode = CurrentMode::NLI;
      
    }
    if (ImGui::MenuItem("BB-Form", nullptr, usingBBform))
    {
      currentMode = CurrentMode::BBForm;
    }
    if (ImGui::MenuItem("Midpoint Subdivision", nullptr, usingMidSub))
    {
      currentMode = CurrentMode::MidpointSub;
    }
    usingNLI = usingBBform = usingMidSub = false;
    if (currentMode == CurrentMode::NLI)
    {
      usingNLI = true;
    }
    else if (currentMode == CurrentMode::BBForm)
    {
      usingBBform = true;
    }
    else
    {
      usingMidSub = true;
    }


    // Make sure to end the menu
    ImGui::EndMenu();

    //Relcalc points on swap
    if (oldNLI != usingNLI)
    {
      CalculatePoints();
    }
  }
  
  // Add more ImGui::BeginMenu(...) for additional menus
}


//NLI recursive func
float Project2::NLIFunc(std::vector<ControlPoint>& points, float t)
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


void Project2::RecalculateShellNLI()
{

  controlPointCopy2D.resize(controlPoints.size());


  //initialize the first points (first row of t values)
  controlPointCopy2D.at(0) = controlPoints;


  for (unsigned i = 1; i < controlPointCopy2D.size(); ++i)
  {
    //resize to the size - i so cascading size
    controlPointCopy2D.at(i).resize(controlPointCopy2D.size() - i);
  }


  for (unsigned i = 1; i < controlPointCopy2D.size(); ++i)
  {
    //Shell Loop
    for (unsigned j = 0; j < controlPointCopy2D.size() - i; ++j)
    {
      controlPointCopy2D[i][j] = controlPointCopy2D[i - 1][j] * (1 - tValueNLI) + controlPointCopy2D[i - 1][j + 1] * tValueNLI;
    }
  }

}

void Project2::CalculatePoints()
{

  if (currentMode == CurrentMode::NLI)
  {
    //NLI
    for (unsigned i = 0; i < quality; ++i)
    {

      //float t = current x value of this point on the graph (the current i / quality as float)
      float t = i / float(quality - 1);

      //p(t) = p[a0,a1,a2,...aN](t) = (1 - t)p[a0,a1,a2,...a(N - 1)](t) + (t)p[a1,a2,...a(N)]
      float p = 0;// = NLIFunc(controlPoints, t);
      int depth = controlPoints.size();

      std::vector<ControlPoint> controlPointCopy = std::vector(controlPoints.begin(), controlPoints.end());


      for (unsigned j = 0; j < depth - 1; ++j)
      {
        for (unsigned k = 0; k < depth - 1; ++k)
        {
          
          controlPointCopy[k] = controlPointCopy[k] * (1 - t) + controlPointCopy[k + 1] * t;
          //Sett the size 

        }
        points[i] = controlPointCopy[0].ToImVec2();
      }
    }
  }
  else if (currentMode == CurrentMode::BBForm)
  {
    //BB-Form BBForm
    for (unsigned i = 0; i < quality; ++i)
    {

      float t = i / float(quality - 1);

      ControlPoint p{ 0,0 };

      //for each point P[i] in array P[Size]
      //gamma(t) = summation from i = 0 to d of (Bernstinen Basis(i,d,t) * P[i]
      for (unsigned j = 0; j < controlPoints.size(); ++j)
      {
        p += controlPoints[j] * BernsteinBasis(j, controlPoints.size() - 1, t);
      }

      points[i] = p.ToImVec2();

    }
  }
  //  if (currentMode == CurrentMode::MidpointSub)
  else
  {
    //Midpoint Subdivision
    //BB-Form BBForm
    for (unsigned i = 0; i < quality; ++i)
    {

      float t = i / float(quality);

      ControlPoint p{ 0,0 };

      //for each point P[i] in array P[Size]
      //gamma(t) = summation from i = 0 to d of (Bernstinen Basis(i,d,t) * P[i]
      for (unsigned j = 0; j < controlPoints.size(); ++j)
      {
        p += controlPoints[j] * BernsteinBasis(j, degree, t);
      }

      points[i] = p.ToImVec2();

    }

  }
}


float Project2::BernsteinBasis(int i, int d, float t)
{
  return float(BernsteinPoly.XChooseY(d, i)) * pow((1.0f - t), (d - i)) * pow((t), (i));
}

std::vector<ControlPoint> Project2::Subdivide(ControlPoint& P1, ControlPoint& P2, ControlPoint& P3, ControlPoint& P4)
{
  ControlPoint P1P2Midpoint = (P1 * 0.5f) + (P2 * 0.5f);
  ControlPoint P2P3Midpoint = (P2 * 0.5f) + (P3 * 0.5f);
  ControlPoint P3P4Midpoint = (P3 * 0.5f) + (P4 * 0.5f);

  return std::vector<ControlPoint>({ P1, P1P2Midpoint, 
    P2, P2P3Midpoint, P3, P3P4Midpoint, P4 });
}

//Returns point inbetween P1 and P2 
ControlPoint Project2::LerpControlPoints(ControlPoint& P1, ControlPoint& P2, float t)
{
  //lerp (1-t) * v1 + t * v2
  return (P1 * (1.0f - t)) + (P2 * t);

}
