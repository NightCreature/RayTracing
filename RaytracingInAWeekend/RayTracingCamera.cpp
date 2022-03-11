#include "RayTracingCamera.h"


///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
void RayTracingCamera::CreateCamera(const Vector3& pos, const Vector3& lookat, const Vector3& up)
{
    auto theta = math::toRadian(m_fov);
    auto h = tan(theta / 2.0);
    auto viewportHeight = 2.0 * h;
    auto aspectRatio = m_width / m_height;
    auto viewportWidth = aspectRatio * viewportHeight;

    m_forward = normalise(pos - lookat);
    m_right = normalise(cross(up, m_forward));
    m_vup = normalise(cross(m_forward, m_right));

    m_origin = pos;
    m_horizontal = m_focusDistance * viewportWidth * m_right;
    m_vertical = m_focusDistance * viewportHeight * m_vup;
    m_lowerLeftCorner = m_origin - m_horizontal / 2 - m_vertical / 2 - m_focusDistance * m_forward;

    m_lensRadius = m_aperature / 2;
}


///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
void RayTracingCamera::CreateCamera(const Vector3& pos, const Vector3& lookat, const Vector3& up, double fov, double imageWidth, double imageHeight, double aperature, double focusDistance)
{
    auto theta = math::toRadian(fov);
    auto h = tan(theta / 2.0);
    auto viewportHeight = 2.0 * h;
    auto aspectRatio = imageWidth / imageHeight;
    auto viewportWidth = aspectRatio * viewportHeight;

    m_forward = normalise(pos - lookat);
    m_right = normalise(cross(up, m_forward));
    m_vup = normalise(cross(m_forward, m_right));

    m_origin = pos;
    m_horizontal = focusDistance * viewportWidth * m_right;
    m_vertical = focusDistance * viewportHeight * m_vup;
    m_lowerLeftCorner = m_origin - m_horizontal / 2 - m_vertical / 2 - focusDistance * m_forward;

    m_lensRadius = aperature / 2;
}
