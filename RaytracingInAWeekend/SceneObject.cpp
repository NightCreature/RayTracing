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
///! @brief   Works seperately not together :?
///! @remark The way this is specified this is a AABB so we already have the hit on this
///-----------------------------------------------------------------------------
bool IntersectWithBox(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord)
{
    AABoundingBox box (object.m_position - (object.m_size / 2), object.m_position + (object.m_size / 2));

    if (box.RayIntersection(ray, tMin, tMax))
    {
        double closestHit = tMax;
        bool hitAnything = false;
        //Need to figure out which plane we hit there are 6 tests here
        //if (IntersectWithXYRect(object, ray, closestHit, tMax, hitRecord, box.m_min.z()))
        //{
        //    hitAnything = true;
        //    closestHit = hitRecord.m_intersectionFactor;
        //}
        //if (IntersectWithXYRect(object, ray, tMin, closestHit, hitRecord, box.m_max.z()))
        //{
        //    hitAnything = true;
        //    closestHit = hitRecord.m_intersectionFactor;
        //}

        if (IntersectWithXZRect(object, ray, tMin, closestHit, hitRecord, box.m_min.y()))
        {
            hitAnything = true;
            closestHit = hitRecord.m_intersectionFactor;
        }
        if (IntersectWithXZRect(object, ray, tMin, closestHit, hitRecord, box.m_max.y()))
        {
            hitAnything = true;
            closestHit = hitRecord.m_intersectionFactor;
        }

        //if (IntersectWithYZRect(object, ray, tMin, closestHit, hitRecord, box.m_min.x()))
        //{
        //    hitAnything = true;
        //    closestHit = hitRecord.m_intersectionFactor;
        //}
        //if (IntersectWithYZRect(object, ray, tMin, closestHit, hitRecord, box.m_max.x()))
        //{
        //    hitAnything = true;
        //    closestHit = hitRecord.m_intersectionFactor;
        //}

        return hitAnything;
    }
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

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool IntersectWithXYRect(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord, double z)
{
    double t = (z - ray.m_origin.z()) / ray.m_direction.z(); //length factor along ray direction to hit the z coord of XYPlane

    if (t < tMin || t > tMax)
    {
        //outside of the interval we are interested in so no collision
        return false;
    }

    Vector3 collisionPoint = ray.PointAtT(t);
    
    double x1 = object.m_position.x() - object.m_size.x();
    double x2 = object.m_position.x() + object.m_size.x();

    double y1 = object.m_position.y() - object.m_size.y();
    double y2 = object.m_position.y() + object.m_size.y();

    if ((collisionPoint.x() < x1 || x2 < collisionPoint.x()) || (collisionPoint.y() < y1 || y2 < collisionPoint.y())) //the coordinates of this point are outside of the square we are interested in
    {
        return false;
    }

    fillOutHitRecord(hitRecord, t, collisionPoint, Vector3::zAxis(), ray, object);
    return true;
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool IntersectWithXZRect(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord, double y)
{
    double t = (y - ray.m_origin.y()) / ray.m_direction.y(); //length factor along ray direction to hit the z coord of XYPlane

    if (t < tMin || t > tMax)
    {
        //outside of the interval we are interested in so no collision
        return false;
    }

    Vector3 collisionPoint = ray.PointAtT(t);

    double x1 = object.m_position.x() - object.m_size.x();
    double x2 = object.m_position.x() + object.m_size.x();

    double z1 = object.m_position.z() - object.m_size.z();
    double z2 = object.m_position.z() + object.m_size.z();

    if ((collisionPoint.x() < x1 || x2 < collisionPoint.x()) || (collisionPoint.z() < z1 || z2 < collisionPoint.z())) //the coordinates of this point are outside of the square we are interested in
    {
        return false;
    }

    fillOutHitRecord(hitRecord, t, collisionPoint, Vector3::yAxis(), ray, object);
    return true;
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool IntersectWithYZRect(const SceneObject& object, const Ray& ray, double& tMin, double& tMax, HitRecord& hitRecord, double x)
{
    double t = (x - ray.m_origin.x()) / ray.m_direction.x(); //length factor along ray direction to hit the z coord of XYPlane

    if (t < tMin || t > tMax)
    {
        //outside of the interval we are interested in so no collision
        return false;
    }

    Vector3 collisionPoint = ray.PointAtT(t);

    double z1 = object.m_position.z() - object.m_size.z();
    double z2 = object.m_position.z() + object.m_size.z();

    double y1 = object.m_position.y() - object.m_size.y();
    double y2 = object.m_position.y() + object.m_size.y();

    if ((collisionPoint.z() < z1 || z2 < collisionPoint.z()) || (collisionPoint.y() < y1 || y2 < collisionPoint.y())) //the coordinates of this point are outside of the square we are interested in
    {
        return false;
    }

    fillOutHitRecord(hitRecord, t, collisionPoint, Vector3::xAxis(), ray, object);

    return true;
}
