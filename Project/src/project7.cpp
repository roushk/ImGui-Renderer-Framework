#include <pch.hpp>

#include "project7.hpp"
#include "project1.hpp"


#include "imgui_distance.hpp"
#include "imgui_disabled.hpp"
#include "imgui_control_point.hpp"

#include <sstream>
#include <iomanip> 
#include "project3.hpp"


Project7::Project7()
{
  //upper left to lower right
  ImGui::SetViewRect({ -0.2f, 3.4f }, { 1.2f, -3.4f });
  reset();
}


void Project7::reset()
{
  BernsteinPoly.SetQuality(maxDegree);
  points.resize(quality);



  ResizeControlPoints();
  drawBox = true;
  drawCircle = true;
  toggleDrawCircle = true;
  circleRadius = 15.f;
}

std::string Project7::name()
{
  return "Project 7";
}

void Project7::ResizeControlPoints()
{
  controlPoints.resize(degree + 1);

  //set x values
  for (unsigned i = 0; i < degree + 1; ++i)
  {
    controlPoints[i].x = (i / (float)degree);
  }

  knotSequence.clear();
  for (unsigned i = 0; i < dValue + controlPoints.size() + 1; ++i)
  {
    knotSequence.push_back(i);

  }
  CalculatePoints();
}

void Project7::DrawFunction()
{
  for (unsigned i = 1; i < points.size(); ++i)
  {
    ImGui::RenderLine(points[i - 1], points[i], ImGui::ColorConvertFloat4ToU32(colorSoftLightGreen));
  }
}


void Project7::draw()
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

  //RecalculateShellNLI();


  if (currentMode == CurrentMode::NLI)
  {

    for (int i = 1; i < controlPointCopy2D.size(); ++i)
    {
      for (int j = 0; j < controlPointCopy2D[i].size() - 1; ++j)
      {
        ImGui::RenderLine(controlPointCopy2D[i][j].ToImVec2(), controlPointCopy2D[i][j + 1].ToImVec2(), blue, lineThickness);
      }
    }
  }

  DrawFunction();
  
}

void Project7::draw_editors()
{
  int oldDegree = degree;
  static ImVec2 windowSize; // Default initializes to { 0, 0 }

  bool changed = false;

  if (oldDegree != degree)
  {
    changed = true;
    //Bounds check degree
    degree = std::max<int>(degree, 1);
    degree = std::min<int>(degree, maxDegree);
    ResizeControlPoints();


  }

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
      
      int S = controlPoints.size() - 1;

      //D range from 1 to NumPoints - 1
      //T range d to N - d
      if (ImGui::SliderInt("D value", &dValue, 1, S))
      {
        changed = true;
        knotSequence.clear();
        for (unsigned i = 0; i < dValue + controlPoints.size() + 1; ++i)
        {
          knotSequence.push_back(i);
        }
      }

      dValue = std::min(dValue, S);
      int N = S + dValue + 1; //N = s + d + 1
      tValueNLI = std::min(std::max(tValueNLI, float(dValue)), float(N - dValue - 0.001f));


      if (ImGui::SliderFloat("T value", &tValueNLI, dValue, N - dValue - 0.001f))
      {
        changed = true;
      }

      if(changed)
      {

        CalculatePoints();
      }




      // Get window size for next drawing
      windowSize = ImGui::GetWindowSize(); // A little hacky, but it works
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
  }


}

void Project7::draw_menus()
{
  CurrentMode oldMode = currentMode;
  // Create drop-down menu button
  if (ImGui::BeginMenu("Project 2 Options"))
  {
    /*
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
    */

    // Make sure to end the menu
    ImGui::EndMenu();

    //Relcalc points on swap
    if (oldMode != currentMode)
    {
      CalculatePoints();
    }
  }
  
  // Add more ImGui::BeginMenu(...) for additional menus
}


//NLI recursive func
float Project7::NLIFunc(std::vector<ControlPoint>& points, float t)
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


void Project7::RecalculateShellNLI()
{
  if (controlPoints.size() == 0)
    return;

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


float Project7::BernsteinBasis(int i, int d, float t)
{
  return float(BernsteinPoly.XChooseY(d, i)) * pow((1.0f - t), (d - i)) * pow((t), (i));
}

std::vector<ControlPoint> Project7::Subdivide(ControlPoint& P1, ControlPoint& P2, ControlPoint& P3, ControlPoint& P4)
{
  ControlPoint P1P2Midpoint = (P1 * 0.5f) + (P2 * 0.5f);
  ControlPoint P2P3Midpoint = (P2 * 0.5f) + (P3 * 0.5f);
  ControlPoint P3P4Midpoint = (P3 * 0.5f) + (P4 * 0.5f);

  return std::vector<ControlPoint>({ P1, P1P2Midpoint,
    P2, P2P3Midpoint, P3, P3P4Midpoint, P4 });
}

//Returns point inbetween P1 and P2 
ControlPoint Project7::LerpControlPoints(ControlPoint& P1, ControlPoint& P2, float t)
{
  //lerp (1-t) * v1 + t * v2
  return (P1 * (1.0f - t)) + (P2 * t);

}

/*
Inputs:
N = num control points
s = N - 1
d = 3 by default, degree value
Points P0-PN
knot sequence of size (N + Degree + 1)


Interval is [td, tN-d]

ex.
t3,t(numPoints - 3)

output func gamma(t)

stage p = 0 to p = 3 (d)
i = J - d + p to J





t value ranges from d to N
 */



void Project7::CalculatePoints()
{
  Project7::points.clear();
  if (controlPoints.size() == 0 || controlPoints.size() == 1)
    return;

  //set of both points in two new vectors
  SetsOfPoints newPoints;
  
  int S = controlPoints.size() - 1;
  dValue = std::max(0, dValue);
  dValue = std::min(dValue, S);
  int N = S + dValue + 1; //N = s + d + 1
  tValueNLI = std::min(std::max(tValueNLI, float(dValue)), float(N - dValue - 0.001f));

  //construct the div diff table to hold the P[p]i points
  std::vector<std::vector<ControlPointDouble>> DivDiffTable(S + 1);

  //initialize the first row to the points
  for (unsigned i = 0; i < S + 1; ++i)
  {
    //same initialization as NLI, decreasing size shell
    DivDiffTable.at(i).resize(S + 1);

  }

  //[0]g = g(0)
  //the [0] values
  //set P[0]i for i = 0 to i = s
  for (unsigned i = 0; i < S + 1; ++i)
  {
    DivDiffTable[0][i].x = controlPoints[i].x;
    DivDiffTable[0][i].y = controlPoints[i].y;
  }

  //pointShell of 0 is the [0]g values
  //pointShell of 1 is the [0,1]g values and so in until its just 1 value
  //the nth vector has the values we want at 0 and end - 1 and the final vector has just 1 value

  /*
    t exists in [td,tN-d]
  findJ that t exists in [tJ,tJ+1)

   */

   //calculate the [0,1] to [0,1...d] values
  int J = tValueNLI; //J is an int where the T value is between J and J + 1, ex for t = 5.5, J = 5
  //int Jmax = N - dValue;

  

  //construct the Newton Form for t values td to tN-d
  for (unsigned i = 0; i < quality; ++i)
  {
    //Need the double precision for more than 16 points

    //set the t value to the delta t between each point and the quality
    double t =  (i * double((controlPoints.size() - 1) / double(quality - 1)));

    for (int p = 1; p <= dValue; ++p)
    {
      //Shell Loop
      for (int i = J - dValue + p; i <= J; ++i)
      {
        /*
         P[p][i] = (t - ti)/(t of (i+d - (p-1)) - ti) * P[p-1][i]
         */
         //assuming that knotSequence[J] =
        DivDiffTable[p][i] =
          DivDiffTable[p - 1][i] * (((J + t) - knotSequence[i]) /
          (knotSequence[i + dValue - (p - 1)] - knotSequence[i])) +
          DivDiffTable[p - 1][i - 1] * ((knotSequence[i + dValue - (p - 1)] - (J + t)) /
          (knotSequence[i + dValue - (p - 1)] - knotSequence[i]));

      }
    }
 
    //explicately use the imvec2 points
    Project7::points.push_back(DivDiffTable[dValue][J].ToImVec2());

  }
}

