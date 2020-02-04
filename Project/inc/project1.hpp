#pragma once
#include "application.hpp"


class BernsteinPolynomial
{
public:
  void SetQuality(int maxDegree)
  {
    if (pyramid.size() < maxDegree + 1)
    {
      pyramid.resize(maxDegree + 1);
    }

    for (unsigned x = 0; x < maxDegree + 1; ++x)
    {

      pyramid[x].resize(maxDegree + 1);

      for (unsigned y = 0; y < maxDegree + 1; ++y)
      {
        pyramid[x][y] = XChooseYCache(x, y);

      }
    }
  }

  int XChooseYCache(int x, int y);
  int XChooseY(int x, int y);
  std::vector<std::vector<int>> pyramid;


};
static BernsteinPolynomial BernsteinPoly;

struct ControlPoint { float x = 1.f, y = 1.f; 
ImVec2 ToImVec2(float _x = 0, float _y = 0)
{
  return{ x + _x,y + _y };
  }
ControlPoint operator* (float rhs) const 
{
  ControlPoint newPoint;
  newPoint.x = x * rhs;
  newPoint.y = y * rhs;
  return newPoint;
}

ControlPoint operator+ (ControlPoint rhs) const
{
  ControlPoint newPoint;
  newPoint.x = x + rhs.x;
  newPoint.y = y + rhs.y;
  return newPoint;
}

ControlPoint& operator+= (const ControlPoint& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}
};





class Project1 : public Project
{
public:
  Project1();

  void draw() final override;
  void draw_editors() final override;
  void draw_menus() final override;

  void reset() final override;

  std::string name() final override;

  //Reset the control points when the degree changes
  void ResizeControlPoints();
  void DrawFunction();
  void CalculatePoints();

  float BernsteinBasis(int i, int d, float t);

private:

  std::vector<ImVec2> points;

  bool drawBox = true;
  bool usingNLI = true;
  int degree = 1;
  const int maxDegree = 100;

  int quality = 100;


  float boxRounding = 0.0f;
  float boxThickness = 5.0f;
  float lineThickness = 1.0f;
  bool drawCircle = true, toggleDrawCircle = true;
  float circleRadius = 15.f;
  int circleDivisions = 128;
  //Control point Y value are the coefficents for the Castlawuhdoawoass alg

  std::vector<ControlPoint> controlPoints{ ControlPoint{2.5f, 1.f}, ControlPoint{5.f, 1.f}, ControlPoint{7.5f, 1.f} };

  // A set of solid colors for drawing onto a dark gray background
  const ImVec4 colorSoftLightGray = { 0.9f, 0.9f, 0.9f, 0.415f };
  const ImVec4 colorSoftDarkGray = { 0.26f, 0.26f, 0.26f, 0.415f };
  const ImVec4 colorSoftLightGreen = { 0.74f, 0.98f, 0.74f, 0.415f };
  const ImVec4 colorSoftBlue = { 0.56f, 0.56f, 0.95f, 1.f };
  const ImVec4 colorSoftWhiteBlue = { 0.74f, 0.74f, 0.98f, 1.f };

  const ImVec4 colorWhite = { 1,1,1,1 };
};
