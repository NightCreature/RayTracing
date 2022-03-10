#pragma once
#include "Ray.h"
#include "Math/vector3.h"
#include "Material.h"


struct HitRecord
{
    void SetSurfaceNormal(const Ray& ray, const Vector3& normal)
    {
        m_hitFrontFace = ray.m_direction.dot(normal) < 0.0;
        m_normal = m_hitFrontFace ? normal : -normal;
    }

    Material m_material;

    Vector3 m_point;
    Vector3 m_normal;

    double m_intersectionFactor;

    bool m_hitFrontFace;
};