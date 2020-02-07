#pragma once
#include "application.hpp"
#include "project1.hpp"


enum class CurrentMode
{
  NLI,
  BBForm,
  MidpointSub

};



class Project2 : public Project
{
public:
  Project2();

  void draw() final override;
  void draw_editors() final override;
  void draw_menus() final override;

  void reset() final override;

  std::string name() final override;

  //Reset the control points when the degree changes
  void ResizeControlPoints();
  void DrawFunction();
  void CalculatePoints();
  void RecalculateShellNLI();

  float NLIFunc(std::vector<ControlPoint>& points, float t);
  float BernsteinBasis(int i, int d, float t);
  std::vector<ControlPoint> Subdivide(ControlPoint& P1, ControlPoint& P2, ControlPoint& P3, ControlPoint& P4);
  ControlPoint LerpControlPoints(ControlPoint& P1, ControlPoint& P2, float t = 0.5f);

private:

  CurrentMode currentMode = CurrentMode::NLI;
  std::vector<ImVec2> points;
  std::vector<std::vector<ControlPoint>> controlPointCopy2D;

  bool drawBox = true;
  
  bool usingNLI = true;
  bool usingBBform = false;
  bool usingMidSub = false;

  int degree = 1;

  float tValueNLI = 0.5f;

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
