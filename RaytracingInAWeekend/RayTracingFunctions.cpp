#include "RayTracingFunctions.h"

#include "HitRecord.h"
#include "MaterialHelpers.h"
#include "Ray.h"
#include "RenderOptions.h"
#include "SceneObject.h"
#include "Math/vector4.h"

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool IntersectWithObjectList(const std::vector<SceneObject>& objectList, const Ray& ray, double tMin, double tMax, HitRecord& record)
{
    bool hitAnything = false;
    double closestHit = tMax;
    HitRecord temp;

    for (auto& object : objectList)
    {
        if (IntersectWithObjectType(object, ray, tMin, closestHit, temp))
        {
            hitAnything = true;
            closestHit = temp.m_intersectionFactor;
            record = temp;
        }
    }

    return hitAnything;
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
color ray_color(const Ray& r, const std::vector<SceneObject>& world, size_t depth)
{
    HitRecord rec;

    if (depth == 0)
    {
        return color(0, 0, 0);
    }

    if (IntersectWithObjectList(world, r, 0.001, std::numeric_limits<double>::infinity(), rec))
    {
        Ray scattered;
        Color attenuation;
        if (ScatterMaterialType(r, rec, attenuation, scattered))
        {
            return attenuation * ray_color(scattered, world, depth - 1);
        }
        //This needs to be the scatter function
        return Color(0, 0, 0);
    }
    Vector3 unit_direction = normalise(r.m_direction);
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
void SingleCoreTracing(std::vector<SceneObject> objectList, const RayTracingCamera& cam, std::vector<Vector4>& imageArray, const RenderOptions& options)
{
    size_t imageWidth = options.m_outputWidth;
    size_t imageHeight = options.m_outputHeight;
    for (int j = 0; j < imageWidth; ++j)
    {
        for (int i = 0; i < imageHeight; ++i)
        {
            for (size_t sampleCount = 0; sampleCount < options.m_numberOfSamples; ++sampleCount)
            {
                auto u = double(i + GetRandomValue()) / (imageWidth - 1);
                auto v = double(j + GetRandomValue()) / (imageHeight - 1);
                Ray r = cam.GetRay(u, v);

                imageArray[imageWidth * j + i] += Vector4(ray_color(r, objectList, options.m_numberOfBounces));
            }
        }
    }
}
