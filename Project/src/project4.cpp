#include <pch.hpp>

#include "project4.hpp"


#include "imgui_distance.hpp"
#include "imgui_disabled.hpp"
#include "imgui_control_point.hpp"

#include <sstream>
#include <iomanip> 


Project4::Project4()
{
  //upper left to lower right
  ImGui::SetViewRect({ -0.2f, 3.4f }, { 1.2f, -3.4f });
  reset();
}


void Project4::reset()
{
  BernsteinPoly.SetQuality(maxDegree);
  points.resize(quality);
  ResizeControlPoints();

  drawBox = false;
  drawCircle = true;
  toggleDrawCircle = true;
  circleRadius = 15.f;
}

std::string Project4::name()
{
  return "Project 4";
}

void Project4::ResizeControlPoints()
{
  controlPoints.resize(degree + 1);

  //set x values
  for (unsigned i = 0; i < degree + 1; ++i)
  {
    controlPoints[i].x = (i / (float)degree);
  }
  CalculatePoints();
}

void Project4::DrawFunction()
{
  for (unsigned i = 1; i < points.size(); ++i)
  {
    ImGui::RenderLine(points[i - 1], points[i], ImGui::ColorConvertFloat4ToU32(colorSoftLightGreen));
  }
}


void Project4::draw()
{
  // Basic colors that work well with the background
  const ImU32 boxColorPacked = ImGui::ColorConvertFloat4ToU32(colorSoftLightGray);
  const ImU32 circleColorPacked = ImGui::ColorConvertFloat4ToU32(colorSoftBlue);
  const ImU32 circleColorHighlightedPacked = ImGui::ColorConvertFloat4ToU32(colorSoftWhiteBlue);
  const ImU32 white = ImGui::ColorConvertFloat4ToU32(colorWhite);
  const ImU32 blue = ImGui::ColorConvertFloat4ToU32(colorSoftBlue);


  if (drawBox)
  {
    ImGui::RenderRect({ 0.0f,  3.0f }, { 1.0f, -3.0f }, boxColorPacked, boxRounding, ImDrawCornerFlags_All, boxThickness);
    
    ImGui::RenderLine({ 0.0f,  2.0f }, { 1.0f,  2.0f }, boxColorPacked, lineThickness);
    ImGui::RenderLine({ 0.0f,  1.0f }, { 1.0f,  1.0f }, boxColorPacked, lineThickness);
    ImGui::RenderLine({ 0.0f,  0.0f }, { 1.0f,  0.0f }, boxColorPacked, lineThickness * 3.0f);
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


  if(drawPointLocations)
  {
    
    for (unsigned i = 0; i < controlPoints.size(); ++i)
    {
      
      std::stringstream pos;
      pos << std::setfill(' ') << std::setw(4) << std::setprecision(2) << float(controlPoints[i].x) << " ," << controlPoints[i].y;
      //std::string pos2 = pos.str();
      ImGui::RenderText(controlPoints[i].ToImVec2(-0.061f,-0.2f), white, pos.str().c_str());

      //Display which number point this is
      //ImGui::RenderText(controlPoints[i].ToImVec2(-0.01f, 0.1f), white, std::to_string(i).c_str());
      
    }
  }
  
  for (int i = 0; i < int(controlPoints.size()) - 1; ++i)
  { 
    ImGui::RenderLine(controlPoints[i].ToImVec2(), controlPoints[i + 1].ToImVec2(), white, lineThickness);
  }


  DrawFunction();
  
}

void Project4::draw_editors()
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
      
      ImGui::Text("Degree = %d", controlPoints.size());

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

void Project4::draw_menus()
{
  // Create drop-down menu button
  if (ImGui::BeginMenu("Project 4 Options"))
  {
    if (ImGui::MenuItem("Newton Form", nullptr, usingNLI))
    {
      
    }
    // Make sure to end the menu
    ImGui::EndMenu();
  }
  
  // Add more ImGui::BeginMenu(...) for additional menus
}




float Project4::BernsteinBasis(int i, int d, float t)
{
  return float(BernsteinPoly.XChooseY(d, i)) * pow((1.0f - t), (d - i)) * pow((t), (i));
}


//Returns point inbetween P1 and P2 
ControlPoint Project4::LerpControlPoints(ControlPoint& P1, ControlPoint& P2, float t)
{
  //lerp (1-t) * v1 + t * v2
  return (P1 * (1.0f - t)) + (P2 * t);

}


void Project4::CalculatePoints()
{
  DividedDifferenceTable(controlPoints);
}



void Project4::DividedDifferenceTable(const std::vector<ControlPoint>& points)
{
  Project4::points.clear();
  //set of both points in two new vectors
  SetsOfPoints newPoints;

  //construct the shell for the points
  std::vector<std::vector<ControlPointDouble>> DivDiffTable(points.size());

  //initialize the first row to the points
  for (unsigned i = 0; i < DivDiffTable.size(); ++i)
  {
    //same initialization as NLI, decreasing size shell
    DivDiffTable.at(i).resize(DivDiffTable.size() - i);

  }

  //[0]g = g(0)
  //the [0] values
  for(unsigned i = 0; i < DivDiffTable.size(); ++i)
  {
    DivDiffTable[0][i].x = points[i].x;
    DivDiffTable[0][i].y = points[i].y;
  }

  //pointShell of 0 is the [0]g values
  //pointShell of 1 is the [0,1]g values and so in until its just 1 value
  //the nth vector has the values we want at 0 and end - 1 and the final vector has just 1 value

  /*
    Input points P0 -> Pd
    t = 0...d

    x(t) and y(t) newton forms
    [a,b]g = ([b]g - [a]g) / (b-a)
    [a]g = g(a) - Base Case
    g(i) = ai
    y(t) g(i) = bi
    i = 0...d

    [ti,ti+1,...ti+k]g = ([ti+1,...,ti+k]g - [ti+1,...,ti+k - 1]g) / ti+k - ti
   */

  //calculate the [0,1] to [0,1...d] values

  for (unsigned i = 1; i < DivDiffTable.size(); ++i)
  {
    //Shell Loop
    for (unsigned j = 0; j < DivDiffTable.size() - i; ++j)
    {
      //User 0.5 as t value


      //[a,b]g = ([b]g - [a]g) / (b-a)
      //where [a,b]g = DivDiffTable[i][j]
      //where [a]g = DivDiffTable[i - 1][j]
      //where [b]g = DivDiffTable[i - 1][j + 1]

      //X value and Y value at once
      DivDiffTable[i][j] = (DivDiffTable[i - 1][j + 1] - DivDiffTable[i - 1][j]) * (1.0/i);

    }
  }



  //construct the Newton Form for all T values 0 -> d
  for(unsigned i = 0; i < quality; ++i)
  {
    //Need the double precision for more than 16 points

    //set the t value to the delta t between each point and the quality
    double t = i * double((controlPoints.size() - 1) / double(quality - 1));

    double currentX = 0;
    double currentY = 0;

    for(unsigned j = 0; j < controlPoints.size(); ++j)
    {
      double delta = 1.0;
      for(unsigned k = 0; k < j; ++k)
      {
        //set the point to its location inside of the range from the start of this point to the end of it
        delta *= t - k;

      }

      //set the current X and Y of this arc between two points
      currentX += delta * double(DivDiffTable[j][0].x);
      currentY += delta * double(DivDiffTable[j][0].y);
    }
    //explicately use the imvec2 points
    Project4::points.push_back(ImVec2{ float(currentX),  float(currentY) });
   
  }

}
