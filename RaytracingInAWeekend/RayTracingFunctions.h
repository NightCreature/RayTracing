#pragma once

#include "Types.h"
#include <vector>
#include "Math/vector4.h"

struct SceneObject;
struct Ray;
struct HitRecord;
struct RenderOptions;
class RayTracingCamera;

bool IntersectWithObjectList(const std::vector<SceneObject>& objectList, const Ray& ray, double tMin, double tMax, HitRecord& record);
color ray_color(const Ray& r, const std::vector<SceneObject>& world, size_t depth);
void SingleCoreTracing(std::vector<SceneObject> objectList, const RayTracingCamera& cam, std::vector<Vector4>& imageArray, const RenderOptions& options);
