#pragma once

#include "Ray.h"
#include "Math/vector3.h"
#include "Utilities.h"


class RayTracingCamera {
public:
    RayTracingCamera() { }
    RayTracingCamera(const Vector3& pos, const Vector3& lookat, const Vector3& up, double fov, double imageWidth, double imageHeight, double aperature, double focusDistance) :
        m_fov(fov), m_width(imageWidth), m_height(imageHeight), m_aperature(aperature), m_focusDistance(focusDistance)
    {
        CreateCamera(pos, lookat, up, fov, imageWidth, imageHeight, aperature, focusDistance);

    }

    void CreateCamera(const Vector3& pos, const Vector3& lookat, const Vector3& up);
    void CreateCamera(const Vector3& pos, const Vector3& lookat, const Vector3& up, double fov, double imageWidth, double imageHeight, double aperature, double focusDistance);

    Ray GetRay(double u, double v) const
    {
        Vector3 rd = m_lensRadius * CreateRandomVectorInUnitDisk();
        Vector3 offset = u * rd.x() + v * rd.y();

        return Ray(m_origin + offset, m_lowerLeftCorner + u * m_horizontal + v * m_vertical - m_origin - offset);
    }

private:
    Vector3 m_origin;
    Vector3 m_lowerLeftCorner;
    Vector3 m_horizontal;
    Vector3 m_vertical;

    Vector3 m_forward;
    Vector3 m_right;
    Vector3 m_vup;

    double m_lensRadius = 0.0;
    double m_fov = 90;
    double m_width = 1280.0;
    double m_height = 720.0;
    double m_aperature = 0.0;
    double m_focusDistance = 1.0;
};