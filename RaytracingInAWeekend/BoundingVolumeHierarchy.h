#pragma once

#include "BoundingBox.h"
#include "SceneObject.h"

struct Ray;
struct HitRecord;

struct BVHVisistStack
{
    size_t numberOfNodes = 0;
    float numberOfLeaves = 0;
};

struct BoundingVolumeHierarchy
{
    BoundingVolumeHierarchy() {}
    BoundingVolumeHierarchy(const std::vector<SceneObject>& objectList) : BoundingVolumeHierarchy(objectList, 0, objectList.size()) {}
    BoundingVolumeHierarchy(const std::vector<SceneObject>& objectList, size_t start, size_t end);
    ~BoundingVolumeHierarchy()
    {
        delete m_left;
        delete m_right;
    }

    bool RayIntersection(const Ray& ray, double tMin, double tMax, HitRecord& record) const;

    void WalkBoudingVolume(BVHVisistStack& output);

    AABoundingBox m_bounds;

    BoundingVolumeHierarchy* m_left = nullptr;
    BoundingVolumeHierarchy* m_right = nullptr;

    //None owning pointer also only leaf objects have this set
    const SceneObject* m_object = nullptr;
};

struct BoundingVolumeHierarchyNode
{
    AABoundingBox m_bounds;
    size_t indexToObject = std::numeric_limits<size_t>::max(); //can't use infinity because integer doesnt have one :(

    //Store indices to where we can find the children in the BVH structure
    size_t m_leftIndex = std::numeric_limits<size_t>::max();
    size_t m_rightIndex = std::numeric_limits<size_t>::max();
};

struct FastBoundingVolumeHierarchy
{
    FastBoundingVolumeHierarchy() {}

    void InsertNodesBFS(const std::vector<SceneObject>& objectList, size_t start, size_t end, size_t currentTreeNodeIndex);
    void WalkBoudingVolumeBFS(BVHVisistStack& output, size_t currentNodeIndex);

    bool RayIntersection(const Ray& ray, double tMin, double tMax, HitRecord& record, size_t currentNodeIndex) const;

    std::vector<BoundingVolumeHierarchyNode> m_nodes;
    const std::vector<SceneObject>* m_objectList = nullptr;
};

inline bool CompareBox(const SceneObject& lhs, const SceneObject& rhs, size_t axis)
{
    auto lhsBox = CreateBoundingBoxForObjectType(lhs);
    auto rhsBox = CreateBoundingBoxForObjectType(rhs);

    return lhsBox.m_min[axis] < rhsBox.m_min[axis];
}

inline bool CompareBoxX(const SceneObject& lhs, const SceneObject& rhs)
{
    return CompareBox(lhs, rhs, 0);
}

inline bool CompareBoxY(const SceneObject& lhs, const SceneObject& rhs)
{
    return CompareBox(lhs, rhs, 1);
}

inline bool CompareBoxZ(const SceneObject& lhs, const SceneObject& rhs)
{
    return CompareBox(lhs, rhs, 2);
}