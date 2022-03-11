#pragma once

#include "SceneObject.h"
#include<vector>

enum class SceneSelection : size_t
{
    RandomSpheres,

    Count
};

struct Scene
{
    void CreateScene(SceneSelection scene);

    std::vector<SceneObject> m_scene;
};


void GenerateScene(std::vector<SceneObject>& objectList, SceneSelection scene);
void GenerateBoudingBoxList(const std::vector<SceneObject>& objectList, std::vector<AABoundingBox>& boundingBoxList);
AABoundingBox GenerateBoundingBoxForList(const std::vector<SceneObject>& objectList);

