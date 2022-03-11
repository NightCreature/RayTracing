#pragma once

#include "Math/vector3.h"
#include "Ray.h"
#include "HitRecord.h"
#include "Material.h"
#include "BoundingBox.h"

enum class ObjectType : size_t
{
    Sphere = 0,
    Box,
    Plane,

    Count
};

struct SceneObject
{
    SceneObject() {}
    SceneObject(const Vector3& pos, const Vector3& size, ObjectType type, const Material& material) : m_position(pos), m_size(size), m_type(type), m_material(material) {}

    Vector3 m_position; 
    Vector3 m_size; //Encode size of object, for spehere all values are the radius, box is xyz dimensions, for a plane this is the normal

    Material m_material;

    ObjectType m_type = ObjectType::Count;
};

bool IntersectWithSphere(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord);
bool IntersectWithBox(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord);
bool IntersectWithPlane(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord);

bool IntersectWithXYRect(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord, double z);
bool IntersectWithXZRect(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord, double y);
bool IntersectWithYZRect(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord, double x);

inline bool IntersectWithObjectType(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord)
{
    switch (object.m_type)
    {
    case ObjectType::Sphere:
        return IntersectWithSphere(object, ray, tMin, tMax, hitRecord);
    case ObjectType::Box:
        return IntersectWithBox(object, ray, tMin, tMax, hitRecord);
    case ObjectType::Plane:

        return IntersectWithPlane(object, ray, tMin, tMax, hitRecord);
    default:
    case ObjectType::Count:
        break;
    }

    return false;
}

inline AABoundingBox CreateBoundingBoxForObjectType(const SceneObject& object)
{
    AABoundingBox retVal;

    switch (object.m_type)
    {
    case ObjectType::Sphere:
    {
        retVal.m_min = object.m_position - object.m_size;
        retVal.m_max = object.m_position + object.m_size;
    } break;
    case ObjectType::Box:
    {
        retVal.m_min = object.m_position - (object.m_size / 2);
        retVal.m_max = object.m_position + (object.m_size / 2);
    } break;
    case ObjectType::Plane:
    {
        retVal.m_min = Vector3();
        retVal.m_max = Vector3();
    } break;
    default:
    case ObjectType::Count:
        break;
    }

    return retVal;
}

///-----------------------------------------------------------------------------
///! @brief  Small helper that gives us the collision info we want
///! @remark inline this when we can
///-----------------------------------------------------------------------------
inline void fillOutHitRecord(HitRecord& hitRecord, double t, const Vector3& pointOnXY, const Vector3& normal, const Ray& ray, const SceneObject& object)
{
    hitRecord.m_intersectionFactor = t;
    hitRecord.m_point = pointOnXY;
    hitRecord.SetSurfaceNormal(ray, normal);
    hitRecord.m_material = object.m_material;
}