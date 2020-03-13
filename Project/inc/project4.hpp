#pragma once
#include "application.hpp"
#include "project1.hpp"
#include "project2.hpp"
#include "project3.hpp"
#include "Mat.hpp"


class Project4 : public Project
{
public:
  Project4();

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
  ControlPoint LerpControlPoints(ControlPoint& P1, ControlPoint& P2, float t = 0.5f);
  
  struct SetsOfPoints
  {
    std::vector<ControlPoint> lhs;
    std::vector<ControlPoint> rhs;
  };

  void DividedDifferenceTable(const std::vector<ControlPoint>& points);


private:
  Mat<double> matrix{ 5,5 };

  std::vector<ImVec2> points;
  std::vector<std::vector<ControlPoint>> controlPointCopy2D;

  bool drawBox = false;
  bool drawPointLocations = false;
  bool usingNLI = true;
  bool usingBBform = false;
  bool usingMidSub = false;

  int degree = 1;

  float tValueNLI = 0.5f;

  int maxSubdivisions = 6;

  const int maxDegree = 80;

  int quality = 200;


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
