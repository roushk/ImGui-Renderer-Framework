#pragma once
#include "application.hpp"
#include "project1.hpp"
#include "project2.hpp"
#include "project3.hpp"
#include "Mat.hpp"
#include "glm/glm.hpp"
#include "Camera.h"


struct ControlPoint3D {
  float x = 1.f;
  float y = 1.f;
  float z = 1.f;

  ControlPoint3D() = default;

  ControlPoint3D(double x_, double y_, double z_)
  {
    x = x_;
    y = y_;
    z = z_;

  }

  glm::vec3 ToVec3(float _x = 0, float _y = 0, float _z = 0)
  {
    return{ x + _x, y + _y , z + _z };
  }

  ControlPoint3D operator* (float rhs) const
  {
    ControlPoint3D newPoint;
    newPoint.x = x * rhs;
    newPoint.y = y * rhs;
    newPoint.z = z * rhs;

    return newPoint;
  }

  ControlPoint3D operator+ (ControlPoint3D rhs) const
  {
    ControlPoint3D newPoint;
    newPoint.x = x + rhs.x;
    newPoint.y = y + rhs.y;
    newPoint.z = z + rhs.z;

    return newPoint;
  }

  ControlPoint3D operator- (ControlPoint3D rhs) const
  {
    ControlPoint3D newPoint;
    newPoint.x = x - rhs.x;
    newPoint.y = y - rhs.y;
    newPoint.z = z - rhs.z;

    return newPoint;
  }

  ControlPoint3D& operator+= (const ControlPoint3D& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;

    return *this;
  }
};


class Project8 : public Project
{
public:
  Project8();

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

  void UpdateCamera();

private:

  std::vector<glm::vec3> points;
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

  std::vector<ControlPoint3D> controlPoints3D{ ControlPoint3D{2.5f, 1.f,0.f}, ControlPoint3D{5.f, 1.f,0.f}, ControlPoint3D{7.5f, 1.f,0.f} };
  std::vector<ControlPoint> controlPoints{ ControlPoint{2.5f, 1.f}, ControlPoint{5.f, 1.f}, ControlPoint{7.5f, 1.f} };


  // A set of solid colors for drawing onto a dark gray background
  const ImVec4 colorSoftLightGray = { 0.9f, 0.9f, 0.9f, 0.415f };
  const ImVec4 colorSoftDarkGray = { 0.26f, 0.26f, 0.26f, 0.415f };
  const ImVec4 colorSoftLightGreen = { 0.74f, 0.98f, 0.74f, 0.415f };
  const ImVec4 colorSoftBlue = { 0.56f, 0.56f, 0.95f, 1.f };
  const ImVec4 colorSoftWhiteBlue = { 0.74f, 0.74f, 0.98f, 1.f };

  const ImVec4 colorWhite = { 1,1,1,1 };





  Camera currentCamera;
  float windowX = 1;
  float windowY = 1;
  float nearPlane = 0.1f;
  float farPlane = 1000.f;

  glm::mat4 projectionMatrix;
  glm::mat4 viewMatrix;


};