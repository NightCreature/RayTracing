#pragma once

#include "SceneObject.h"
#include<vector>
#include "BoundingVolumeHierarchy.h"
#include "RayTracingCamera.h"

struct RenderOptions;

enum class SceneSelection : size_t
{
    RandomSpheres,
    Boxes,

    Count
};

struct Scene
{
    Scene() {}
    Scene(const RayTracingCamera& cam) : m_camera(cam) {}
    Scene(const RayTracingCamera& cam, SceneSelection scene) : m_camera(cam) { CreateScene(scene); }
    void CreateScene(SceneSelection scene);

    std::vector<SceneObject> m_scene;
    RayTracingCamera m_camera;
    FastBoundingVolumeHierarchy m_boundingVolume;

    RenderOptions* m_renderOptions = nullptr;
};


void GenerateScene(std::vector<SceneObject>& objectList, RayTracingCamera& camera, SceneSelection scene, const RenderOptions& options);
void GenerateBoudingBoxList(const std::vector<SceneObject>& objectList, std::vector<AABoundingBox>& boundingBoxList);
AABoundingBox GenerateBoundingBoxForList(const std::vector<SceneObject>& objectList);

