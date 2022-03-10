#pragma once

#include "SceneObject.h"
#include<vector>

struct Scene
{
    void CreateScene();

    std::vector<SceneObject> m_scene;
};

void GenerateScene(std::vector<SceneObject>& objectList);
void GenerateBoudingBoxList(const std::vector<SceneObject>& objectList, std::vector<AABoundingBox>& boundingBoxList);
AABoundingBox GenerateBoundingBoxForList(const std::vector<SceneObject>& objectList);

