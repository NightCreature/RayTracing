#include "SceneObject.h"

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool IntersectWithSphere(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord)
{
    Vector3 oc = ray.m_origin - object.m_position;
    auto a = ray.m_direction.dot(ray.m_direction);
    auto half_b = oc.dot( ray.m_direction);
    auto c = oc.dot(oc) - object.m_size.x() * object.m_size.x();

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < tMin || tMax < root)
    {
        root = (-half_b + sqrtd) / a;
        if (root < tMin || tMax < root)
            return false;
    }

    hitRecord.m_intersectionFactor = root;
    hitRecord.m_point = ray.PointAtT(hitRecord.m_intersectionFactor);
    hitRecord.SetSurfaceNormal(ray, (hitRecord.m_point - object.m_position) / object.m_size.x()); //normalised normal
    hitRecord.m_material = object.m_material;

    return true;
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark this is like colliding with 6 planes with some definitions
///-----------------------------------------------------------------------------
bool IntersectWithBox(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord)
{
    return false;
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool IntersectWithPlane(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord)
{
    return false;
}
