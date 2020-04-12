#include <pch.hpp>

#include "Project8.hpp"


#include "imgui_distance.hpp"
#include "imgui_disabled.hpp"
#include "imgui_control_point.hpp"

#include <sstream>
#include <iomanip> 
#include "Projection.h"


Project8::Project8()
{
  //upper left to lower right
  ImGui::SetViewRect({ -0.2f, 3.4f }, { 1.2f, -3.4f });
  reset();
  currentCamera = Camera({ 0,0,-5 }, { 0,0,-1 }, { 0,1,0 }, 90, 1, nearPlane, farPlane);
}


void Project8::reset()
{
  UpdateCamera();

  BernsteinPoly.SetQuality(maxDegree);
  points.resize(quality);
  ResizeControlPoints();

  drawBox = false;
  drawCircle = true;
  toggleDrawCircle = true;
  circleRadius = 15.f;

  CalculatePoints();

}

std::string Project8::name()
{
  return "Project 8";
}

void Project8::ResizeControlPoints()
{
  controlPoints.resize(degree + 1);

  //set x values
  for (unsigned i = 0; i < degree + 1; ++i)
  {
    controlPoints[i].x = (i / (float)degree);
  }
  //CalculatePoints();
}



//proj * view * model * vert
void Project8::UpdateCamera()
{
  

  projectionMatrix = cameraToNDC(currentCamera, windowX, windowY, nearPlane, farPlane);
  viewMatrix = worldToCamera(currentCamera);
}



void Project8::DrawFunction()
{
  std::vector<ImVec2> points2D;
  points2D.reserve(points.size());

  for(auto& pt: points)
  {
    //proj * view * model * vert
    //vert /= vert.w


    //persp proj cpu side
    glm::vec4 vert = projectionMatrix * viewMatrix * glm::vec4(pt,1);

    //perspective divide
    vert /= -vert.w;
    points2D.push_back({ vert.x,vert.y });
  }

  for (unsigned i = 1; i < points.size(); ++i)
  {
    ImGui::RenderLine(points2D[i - 1], points2D[i], ImGui::ColorConvertFloat4ToU32(colorSoftLightGreen));
  }
}


void Project8::draw()
{

  UpdateCamera();

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

  /*
   *if(Camera moved, point places, settings changed)
   *  recalculate 3D control points to 2D control point positions
   *
   */

  if (drawPointLocations)
  {

    for (unsigned i = 0; i < controlPoints.size(); ++i)
    {

      std::stringstream pos;
      pos << std::setfill(' ') << std::setw(4) << std::setprecision(2) << float(controlPoints[i].x) << " ," << controlPoints[i].y;
      //std::string pos2 = pos.str();
      ImGui::RenderText(controlPoints[i].ToImVec2(-0.061f, -0.2f), white, pos.str().c_str());

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

void Project8::draw_editors()
{
  
  float dt = ImGui::GetIO().DeltaTime;
  const float speed = 10.0f * dt;

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

  if(displayCameraControls)
  {
    ImGui::Begin("CameraControls");
   
    if(ImGui::Button("Pan Left"))
    {
      currentCamera.leftRight(speed);
    }
    ImGui::SameLine();
    if(ImGui::Button("Pan Right"))
    {
      currentCamera.leftRight(-speed);
    }

    if(ImGui::Button("Pan Forward"))
    {
      currentCamera.forward(-speed);
    }

    ImGui::SameLine();
    if(ImGui::Button("Pan Backward"))
    {
      currentCamera.forward(speed * 2.0f);
    }

    if(ImGui::Button("Pan Up"))
    {
      currentCamera.upDown(-speed * 2.0f);
    }

    
    ImGui::SameLine();
    if(ImGui::Button("Pan Down"))
    {
      currentCamera.upDown(speed);
    }

    if(ImGui::Button("Rotate Left"))
    {
      currentCamera.yaw(speed * 2.0f);
    }
    ImGui::SameLine();
    if(ImGui::Button("Rotate Right"))
    {
      currentCamera.yaw(-speed * 2.0f);
    }

    if(ImGui::Button("Rotate Up"))
    {
      currentCamera.pitch(speed* 2.0f);
    }
    ImGui::SameLine();
    if(ImGui::Button("Rotate Down"))
    {
      currentCamera.pitch(-speed * 2.0f);
    }

    if(ImGui::Button("Roll Left"))
    {
      currentCamera.roll(speed * 0.5f);
    }
    ImGui::SameLine();
    if(ImGui::Button("Roll Right"))
    {
      currentCamera.roll(-speed * 0.5f);
    }

    if (ImGui::Button("Reset Roll"))
    {
      currentCamera.ResetRoll();
    }
    ImGui::End();

  }

  if (oldDegree != degree)
  {

    //Bounds check degree
    degree = std::max<int>(degree, 1);
    degree = std::min<int>(degree, maxDegree);
    ResizeControlPoints();
  }
}

void Project8::draw_menus()
{
  // Create drop-down menu button
  if (ImGui::BeginMenu("Project 8 Options"))
  {
    if (ImGui::MenuItem("Camera Controls", nullptr, displayCameraControls))
    {

    }
    // Make sure to end the menu
    ImGui::EndMenu();
  }

  // Add more ImGui::BeginMenu(...) for additional menus
}




float Project8::BernsteinBasis(int i, int d, float t)
{
  return float(BernsteinPoly.XChooseY(d, i)) * pow((1.0f - t), (d - i)) * pow((t), (i));
}


//Returns point inbetween P1 and P2 
ControlPoint Project8::LerpControlPoints(ControlPoint& P1, ControlPoint& P2, float t)
{
  //lerp (1-t) * v1 + t * v2
  return (P1 * (1.0f - t)) + (P2 * t);

}


void Project8::CalculatePoints()
{
  //N = control points size
  //K = N - 1
  //D = N + 2
  points.clear();

  int N = controlPoints.size();
  int K = N - 1;
  int D = N + 2;

  Mat<double> matrixX(D, D + 1, 0);


  // create (N + 2) x (N + 3) matrix

  //first 4 terms

  /*
   * form of:
   * 1, t, t^2, t^3, (t - 1)^3+, (t - 2)^3+, ... , (t - (k - 1))^3+
   * .
   * . for all t of integer values [0, k]
   * .
   * 0, 0, 2, 0, 0, ... , 0
   * 0, 0, 2, 6k, 6(t - 1)+, 6(t - 2)+, ... , 6(t - (k - 1))+
   */

  matrixX(0, 0) = 1;
  for (int i = 0; i < N; ++i)
  {
    matrixX(i, 0) = 1;
    matrixX(i, 1) = i;
    matrixX(i, 2) = i * i;
    matrixX(i, 3) = i * i * i;

    for (int j = 4; j < D; ++j)
    {
      //truncated power function
      if (i < j - 3)
        matrixX(i, j) = 0;
      else
        matrixX(i, j) = pow((i - (j - 3)), 3);
    }
  }

  //last 2 rows
  matrixX(D - 2, 2) = 2;
  matrixX(D - 1, 2) = 2;
  matrixX(D - 1, 3) = 6 * K;

  for (int i = 4; i < D; ++i)
  {
    //truncated power function where c = i - 3
    if (K < i - 3)
      matrixX(D - 1, i) = 0;
    else
      matrixX(D - 1, i) = 6.0f * (K - (i - 3.0f));
  }

  //init matrix to the same because they are the same functions minute the control points
  Mat<double> matrixY{ matrixX };
  Mat<double> matrixZ{ matrixX };

  for (int i = 0; i < N; ++i)
  {
    matrixX(i, D) = controlPoints[i].x;
    matrixY(i, D) = controlPoints[i].y;
    matrixZ(i, D) = 0;

    //matrixX(i, D) = controlPoints3D[i].x;
    //matrixY(i, D) = controlPoints3D[i].y;
    //matrixZ(i, D) = controlPoints3D[i].z;

  }

  /*
  ImGui::SetNextWindowSize({ 0.f, 0.f });
  if(ImGui::Begin("Matrix X"))
  {
    for(int i = 0; i < D; ++i)
    {
      for (int j = 0; j < D + 1; ++j)
      {
        ImGui::Text(fmt::format("{}", matrixX(i, j)).c_str());
        if (j != D)
          ImGui::SameLine();
      }
    }
    ImGui::End();
  }
  */


  // Set last column of matrix to control point x and y values (can make 2 matrices pretty trivially)
  matrixX.inplace_rref();
  matrixY.inplace_rref();
  matrixZ.inplace_rref();

  // rref to find coefficients a0, a1, a2, a3, b0, b1, ... , bk-1

  // plug in all t values from [0, k] into spline equation to get final points

  double a0x = matrixX(0, D);
  double a1x = matrixX(1, D);
  double a2x = matrixX(2, D);
  double a3x = matrixX(3, D);

  double a0y = matrixY(0, D);
  double a1y = matrixY(1, D);
  double a2y = matrixY(2, D);
  double a3y = matrixY(3, D);


  double a0z = matrixZ(0, D);
  double a1z = matrixZ(1, D);
  double a2z = matrixZ(2, D);
  double a3z = matrixZ(3, D);

  std::vector<double> bx;
  std::vector<double> by;
  std::vector<double> bz;

  //push back coefficients
  for (int i = 4; i < D; ++i)
  {
    bx.push_back(matrixX(i, D));
    by.push_back(matrixY(i, D));
    bz.push_back(matrixZ(i, D));

  }

  for (int i = 0; i < quality; ++i)
  {
    double t = (i * K) / (quality - 1.0);

    double x = a0x + a1x * t + a2x * t * t + a3x * t * t * t;
    double y = a0y + a1y * t + a2y * t * t + a3y * t * t * t;
    double z = a0z + a1z * t + a2z * t * t + a3z * t * t * t;

    for (int j = 0; j < D - 4; ++j)
    {
      if (t < j + 1)
      {
        x += 0;
        y += 0;
        z += 0;

      }
      else
      {
        x += bx[j] * pow((t - (j + 1.0)), 3);
        y += by[j] * pow((t - (j + 1.0)), 3);
        z += bz[j] * pow((t - (j + 1.0)), 3);

      }
    }


    Project8::points.push_back(glm::vec3{ float(x),  float(y) , float(z)});
  }
}


