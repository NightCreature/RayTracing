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

class RayTracingCamera {
public:
    RayTracingCamera()
    {
        auto aspect_ratio = 16.0 / 9.0;
        auto viewport_height = 2.0;
        auto viewport_width = aspect_ratio * viewport_height;
        auto focal_length = 1.0;

        origin = Vector3(0, 0, 0);
        horizontal = Vector3(viewport_width, 0.0, 0.0);
        vertical = Vector3(0.0, viewport_height, 0.0);
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vector3(0, 0, focal_length);
    }

    RayTracingCamera(double fov, double aspectRatio)
    {
        auto theta = math::toRadian(fov);
        auto h = tan(theta / 2.0);
        auto viewportHeight = 2 * h;
        auto viewportWidth = aspectRatio * viewportHeight;

        auto focalLength = 1.0;

        origin = Vector3(0, 0, 0);
        horizontal = Vector3(viewportWidth, 0.0, 0.0);
        vertical = Vector3(0.0, viewportHeight, 0.0);
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vector3(0, 0, focalLength);
    }

    RayTracingCamera(Vector3 pos, Vector3 lookat, Vector3 up, double fov, double imageWidth, double imageHeight, double aperature, double focusDistance)
    {
        auto theta = math::toRadian(fov);
        auto h = tan(theta / 2.0);
        auto viewportHeight = 2.0 * h;
        auto aspectRatio = imageWidth / imageHeight;
        auto viewportWidth = aspectRatio * viewportHeight;

        forward = normalise(pos - lookat);
        right = normalise(cross(up, forward));
        vup = normalise(cross(forward, right));

        origin = pos;
        horizontal = focusDistance * viewportWidth * right;
        vertical = focusDistance * viewportHeight * vup;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focusDistance * forward;

        lensRadius = aperature / 2;
    }

    Ray GetRay(double u, double v) const
    {
        Vector3 rd = lensRadius * CreateRandomVectorInUnitDisk();
        Vector3 offset = u * rd.x() + v * rd.y();

        return Ray(origin + offset, lower_left_corner + u * horizontal + v * vertical - origin - offset);
    }

private:
    Vector3 origin;
    Vector3 lower_left_corner;
    Vector3 horizontal;
    Vector3 vertical;

    Vector3 forward;
    Vector3 right;
    Vector3 vup;

    double lensRadius = 0.0;
};