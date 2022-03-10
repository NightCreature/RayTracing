#pragma once

#include "Math/vector3.h"
#include "Ray.h"

#include <utility>
#include <corecrt_math.h>
#include <limits>

class BoundingBox
{
};

struct AABoundingBox
{
    AABoundingBox() {}
    AABoundingBox(const Vector3& min, const Vector3& max) : m_min(min), m_max(max) {}

    AABoundingBox(const AABoundingBox& box1, const AABoundingBox& box2)
    {
        m_min = Vector3(std::min(box1.m_min.x(), box2.m_min.x()), std::min(box1.m_min.y(), box2.m_min.y()), std::min(box1.m_min.z(), box2.m_min.z()));
        m_max = Vector3(std::max(box1.m_max.x(), box2.m_max.x()), std::max(box1.m_max.y(), box2.m_max.y()), std::max(box1.m_max.z(), box2.m_max.z()));
    }

    bool RayIntersection(const Ray& ray, double tMin, double tMax) const
    {
        for (size_t coordIndex = 0; coordIndex < 3; ++coordIndex)
        {
            double roci = ray.m_origin[coordIndex];
            double invRDCI = 1.0 / ray.m_direction[coordIndex];
            double t0 = std::min((m_min[coordIndex] - roci) * invRDCI, (m_max[coordIndex] - roci) * invRDCI);
            double t1 = std::max((m_min[coordIndex] - roci) * invRDCI, (m_max[coordIndex] - roci) * invRDCI);
            tMin = std::max(t0, tMin);
            tMax = std::min(t1, tMax);
            if (tMax <= tMin)
            {
                return false;
            }
        }

        return true;
    }


    bool RayIntersectionOptimised(const Ray& ray, double tMin, double tMax) const
    {
        for (size_t coordIndex = 0; coordIndex < 3; coordIndex++)
        {
            double invD = 1.0 / ray.m_direction[coordIndex];
            double t0 = (m_min[coordIndex] - ray.m_origin[coordIndex]) * invD;
            double t1 = (m_max[coordIndex] - ray.m_origin[coordIndex]) * invD;
            if (invD < 0.0)
            {
                std::swap(t0, t1);
            }
            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if (tMax <= tMin)
            {
                return false;
            }
        }
        return true;
    }

    Vector3 m_min = Vector3(std::numeric_limits<double>::infinity());
    Vector3 m_max = Vector3(-std::numeric_limits<double>::infinity());
};