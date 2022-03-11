#pragma once

#include "Math/vector4.h"
#include "Math/vector3.h"
#include "Utilities.h"
#include "Math/matrixmath.h"

struct RenderOptions;

struct Ray
{
    Ray() {}
    Ray(const Vector3& origin, const Vector3& dir) : m_origin(origin), m_direction(dir) {}
    Vector3 m_origin;
    Vector3 m_direction;

    void CreateRay(const RenderOptions& options, size_t x, size_t y);

    const Vector3 PointAtT(double t) const { return m_origin + m_direction * t; }
};
