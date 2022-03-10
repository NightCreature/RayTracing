#include "PixelBlockJob.h"

#include "RenderOptions.h"
#include "Utilities.h"
#include "MaterialHelpers.h"
#include "BoundingVolumeHierarchy.h"
#include <Optick.h>

bool IntersectWithObjectListJobBased(const std::vector<SceneObject>& objectList, const Ray& ray, double tMin, double tMax, HitRecord& record)
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

Color RayColorJobBased(const Ray& r, const std::vector<SceneObject>& world, size_t depth)
{
    HitRecord rec;

    if (depth == 0)
    {
        return color(0, 0, 0);
    }

    if (IntersectWithObjectListJobBased(world, r, 0.001, std::numeric_limits<double>::infinity(), rec))
    {
        Ray scattered;
        Color attenuation;
        if (ScatterMaterialType(r, rec, attenuation, scattered))
        {
            return attenuation * RayColorJobBased(scattered, world, depth - 1);
        }
        //This needs to be the scatter function
        return Color(0, 0, 0);
    }
    Vector3 unit_direction = normalise(r.m_direction);
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

//
//bool IntersectWithBVHJobBased(const BoundingVolumeHierarchy& bvh, const Ray& ray, double tMin, double tMax, HitRecord& record)
//{
//    bool hitAnything = false;
//    double closestHit = tMax;
//    HitRecord temp;
//
//    if (bvh.RayInterSection(ray, tMin, tMax, record))
//    {
//
//    }
//
//    for (auto& object : objectList)
//    {
//        if (IntersectWithObjectType(object, ray, tMin, closestHit, temp))
//        {
//            hitAnything = true;
//            closestHit = temp.m_intersectionFactor;
//            record = temp;
//        }
//    }
//
//    return hitAnything;
//}

//this has errors need to figure this out
Color RayColorBVHJobBased(const Ray& r, const FastBoundingVolumeHierarchy& bvh, size_t depth)
{
    OPTICK_EVENT();

    HitRecord rec;

    if (depth == 0) //THis only happens at the end of the bounces and if you happen to miss all objects before that you bring in the environment color through the exit at the bottom
    {
        return Color(0.0);
    }

    if (bvh.RayIntersection(r, 0.001, std::numeric_limits<double>::infinity(), rec, 0))
    {
        Ray scattered;
        Color attenuation;
        Color emitted = rec.m_material.m_emitted;
        if (ScatterMaterialType(r, rec, attenuation, scattered))
        {
            return emitted + attenuation * RayColorBVHJobBased(scattered, bvh, depth - 1);
        }
        //This needs to be the scatter function
        return Color(0.1);
    }
    Vector3 unit_direction = normalise(r.m_direction);
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
    return Color(0.1);
}


///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
void PixelBlockJob::Execute(size_t threadIndex)
{
    OPTICK_EVENT();

    m_timer.update();
    size_t now = m_timer.getTimeStamp();
    std::stringstream str("");
    str << "<<<<<" << threadIndex << ">>>>>\n";
    str << "Starting PixelJob on Thread " << threadIndex << "\n";
    str << "With Settings:\n" << "Start Row: " << m_params.m_startIndex << "\nEnd Row: " << m_params.m_endIndex <<"\n";
    str << "<<<<<" << threadIndex << ">>>>>\n";
    TraceToOuput(str.str());
    size_t width = m_params.m_renderOptions->m_outputWidth;
    size_t noSamples = m_params.m_renderOptions->m_numberOfSamples;
    std::vector<Vector4> threadLocalArray;
    size_t numberOfRowsToGenerate = m_params.m_endIndex - m_params.m_startIndex;
    //threadLocalArray.resize(width * numberOfRowsToGenerate);
    auto& pixelArrayRef = *(m_params.m_pixelArray);
    std::vector<SceneObject>& scene = *(m_params.scene);
    

    //std::vector<size_t> pixelTime;
    //pixelTime.resize(width * numberOfRowsToGenerate);

    //str.str("");

    for (int j = 0; j < numberOfRowsToGenerate; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            for (size_t sampleCount = 0; sampleCount < noSamples; ++sampleCount)
            {
                OPTICK_EVENT("Sample Pixel");
                auto u = double(i + GetRandomValue()) / (m_params.m_renderOptions->m_outputWidth - 1);
                auto v = double(j + m_params.m_startIndex + GetRandomValue()) / (m_params.m_renderOptions->m_outputHeight - 1);
                Ray r = m_params.m_renderOptions->m_cam.GetRay(u, v);

                pixelArrayRef[m_params.m_renderOptions->m_outputWidth * (j + m_params.m_startIndex) + i] += Vector4(RayColorBVHJobBased(r, *m_params.m_renderOptions->m_fastBVH, m_params.m_renderOptions->m_numberOfBounces));
                //pixelArrayRef[m_params.m_renderOptions->m_outputWidth * (j + m_params.m_startIndex) + i] += Vector4(RayColorJobBased(r, scene, m_params.m_renderOptions->m_numberOfBounces));
            }
        }
    }

   
    //memcpy(&(pixelArrayRef[m_params.m_startIndex]), &(threadLocalArray[0]), sizeof(Vector4) * numberOfRowsToGenerate * width);

    size_t end = m_timer.getTimeStamp();
    //OutputDebugString(str.str().c_str());
    str.str("");
    str << "<<<<<" << threadIndex << ">>>>>\n";
    //for (size_t interval : pixelTime)
    //{
    //    str << "Single ray took: " << interval / m_timer.getResolution() << "s" << "\n";
    //}
    str << "Finished PixelJob on Thread " << threadIndex << " in: " << ConvertTimeDurationToString((end - now) / m_timer.getResolution()).c_str() << "s\n";
    str << "<<<<<" << threadIndex << ">>>>>\n";
    TraceToOuput(str.str());
}
