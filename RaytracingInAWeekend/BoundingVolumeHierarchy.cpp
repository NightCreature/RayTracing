#include "BoundingVolumeHierarchy.h"

#include "HitRecord.h"
#include "Ray.h"
#include "SceneObject.h"
#include <sstream>
#include <limits>
#include <Optick.h>

///-----------------------------------------------------------------------------
///! @brief   
///! @remark this is not an optimal solution it copies the list like crazy, might be better to just pre sort the list and just insert here
///-----------------------------------------------------------------------------
BoundingVolumeHierarchy::BoundingVolumeHierarchy(const std::vector<SceneObject>& objectList, size_t start, size_t end)
{
    OPTICK_EVENT();

    size_t axis = GetRandomUInt(0, 2);

    auto comparator = (axis == 0) ? CompareBoxX : ((axis == 1) ? CompareBoxY : CompareBoxZ);

    size_t objectSpan = end - start;

    //Stop recursion
    if (objectSpan == 1)
    {
        m_object = &(objectList[start]);
    }
    else if (objectSpan == 2)
    {
        m_left = new BoundingVolumeHierarchy();
        m_right = new BoundingVolumeHierarchy();

        if (comparator(objectList[start], objectList[start + 1]))
        {
            m_left->m_object = &objectList[start];
            m_right->m_object = &objectList[start + 1];
        }
        else
        {
            m_left->m_object = &objectList[start + 1];
            m_right->m_object = &objectList[start];
        }

        m_left->m_bounds = CreateBoundingBoxForObjectType(*m_left->m_object);
        m_right->m_bounds = CreateBoundingBoxForObjectType(*m_right->m_object);
    }
    else //Recurse and split the list halfway
    {
        auto list = objectList; //We need to sort this list
        std::sort(list.begin() + start, list.begin() + end, comparator); //this makes the memory random

        size_t mid = start + objectSpan / 2;
        m_left = new BoundingVolumeHierarchy(objectList, start, mid);
        m_right = new BoundingVolumeHierarchy(objectList, mid, end);
    }

    //Create the bounding box here
    if (m_left && m_right)
    {
        m_bounds = AABoundingBox(m_left->m_bounds, m_right->m_bounds);
    }
    else
    {
        m_bounds = CreateBoundingBoxForObjectType(*m_object);
    }
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool BoundingVolumeHierarchy::RayIntersection(const Ray& ray, double tMin, double tMax, HitRecord& record) const 
{
    if (m_bounds.RayIntersection(ray, tMin, tMax))
    {
        if (m_left && m_right) //There are no nodes that have either left or right set to a valid ptr, we are at the object in that case and we can trace against it
        {
            bool hitLeft = m_left->RayIntersection(ray, tMin, tMax, record);
            bool hitRight = m_right->RayIntersection(ray, tMin, (hitLeft ? record.m_intersectionFactor : tMax), record);

            return hitLeft || hitRight;
        }
        else
        {
            //Actually intersect with the object
            return IntersectWithObjectType(*m_object, ray, tMin, tMax, record);
        }
    }

    return false;
}

static size_t WalkVisited = 0;
///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
void BoundingVolumeHierarchy::WalkBoudingVolume(BVHVisistStack& output)
{
    ++output.numberOfNodes;

    std::stringstream str;
    str << "Bounds of Node: min: " << m_bounds.m_min << ", max: " << m_bounds.m_max << "\n";
    TraceToOuput(str.str());
    str.str("");

    if (m_left && m_right) //There are no nodes that have either left or right set to a valid ptr, we are at the object in that case and we can trace against it
    {
        //str << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n";
        //str << "Object in BVH: another node\n";
        //str << "Bounds min: " << m_bounds.m_min << " max: " << m_bounds.m_max << "\n";
        m_left->WalkBoudingVolume(output);
        m_right->WalkBoudingVolume(output);
        //str << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n";
        str << "Not a leaf, nr nodes: " << output.numberOfNodes << "\n";
    }
    else
    {
        //str << "----------------------------------------------------------------------------------------------------\n";
        //Actually intersect with the object
        //str << "Object in BVH with material type: " << static_cast<std::underlying_type_t<MaterialType>>(m_object->m_material.m_type) << "\n";
        //str << "Bounds min: " << m_bounds.m_min << " max: " << m_bounds.m_max << "\n";
        //str << "----------------------------------------------------------------------------------------------------\n";
        ++output.numberOfLeaves;
        

        str << "Leaf, nr leaves: " << output.numberOfLeaves << "\n";
    }

    str << "\n";

    TraceToOuput(str.str());

    //++WalkVisited;
    //str << "Walk Called: " << WalkVisited << "\n";
    //TraceToOuput(str.str());
}

//bad shit in here 
///-----------------------------------------------------------------------------
///! @brief Breadth First Search
///! @remark left = nodeIndex * 2 + 1; right = nodeIndex * 2 + 2
///-----------------------------------------------------------------------------
void FastBoundingVolumeHierarchy::InsertNodesBFS(const std::vector<SceneObject>& objectList, size_t start, size_t end, size_t currentTreeNodeIndex)
{
    //Have to do some work when we first start this
    if (currentTreeNodeIndex == 0)
    {
        TraceToOuput("resize node array");
        m_nodes.resize(objectList.size() * 4 + 10); //there are a lot of nodes above you that are your parents since this is a tree with all data at the bottom
        m_objectList = &objectList;
    }

    size_t axis = GetRandomUInt(0, 2);

    size_t objectSpan = end - start; //total amount of  nodes under this one, left and right will have half of these
    auto& node = m_nodes[currentTreeNodeIndex];

    //Stop recursion
    if (objectSpan == 1) //Single node to insert
    {
        node.indexToObject = start;
        node.m_bounds = CreateBoundingBoxForObjectType(objectList[start]);
        //Don't touch left and right
    }
    else if (objectSpan == 2)
    {
        size_t leftIndex = start;
        size_t rightIndex = start + 1;
        if (!CompareBox(objectList[leftIndex], objectList[rightIndex], axis))
        {
            leftIndex = start + 1;
            rightIndex = start;
        }

        node.m_leftIndex = currentTreeNodeIndex * 2 + 1;
        node.m_rightIndex = currentTreeNodeIndex * 2 + 2;
        auto& leftNode = m_nodes[node.m_leftIndex];
        auto& rightNode = m_nodes[node.m_rightIndex];

        leftNode.m_bounds = CreateBoundingBoxForObjectType(objectList[leftIndex]);
        leftNode.indexToObject = leftIndex;
        rightNode.m_bounds = CreateBoundingBoxForObjectType(objectList[rightIndex]);
        rightNode.indexToObject = rightIndex;

        node.m_bounds = AABoundingBox(m_nodes[node.m_leftIndex].m_bounds, m_nodes[node.m_rightIndex].m_bounds);
    }
    else //Recurse and split the list halfway
    {
        std::vector<size_t> objectIndexList;
        objectIndexList.resize(objectList.size());
        for (size_t index = 0; index < objectList.size(); ++index)
        {
            objectIndexList[index] = index;
        }

        //std::sort(objectIndexList.begin() + start, objectIndexList.begin() + end, [&objectList, &axis](const auto& lhs, const auto& rhs) {return CompareBox(objectList[lhs], objectList[rhs], axis); });


        node.m_leftIndex = currentTreeNodeIndex * 2 + 1;
        node.m_rightIndex = currentTreeNodeIndex * 2 + 2;

        size_t mid = start + objectSpan / 2;
        InsertNodesBFS(objectList, start, mid, node.m_leftIndex); //figure out if we can know where this will store
        InsertNodesBFS(objectList, mid, end, node.m_rightIndex); //figure out if we can know where this will store
        node.m_bounds = AABoundingBox(m_nodes[node.m_leftIndex].m_bounds, m_nodes[node.m_rightIndex].m_bounds);
    }
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
void FastBoundingVolumeHierarchy::WalkBoudingVolumeBFS(BVHVisistStack& output, size_t currentNodeIndex)
{
    std::stringstream str;
    constexpr size_t maxSizeT = std::numeric_limits<size_t>::max();
    for (size_t index = 0; index < m_nodes.size(); ++index)
    {
        bool isLeaf = (m_nodes[index].indexToObject != maxSizeT);
        str << "Index: " << index << " left: " << m_nodes[index].m_leftIndex << ", right: " << m_nodes[index].m_rightIndex << ", leaf: " << (isLeaf ? "true" : "false") << ", object index: " << (isLeaf ? m_nodes[index].indexToObject : maxSizeT)
            << ",min: " << m_nodes[index].m_bounds.m_min << ", max" << m_nodes[index].m_bounds.m_max << "\n";
        if (index % 256 == 0)
        {
            TraceToOuput(str.str());
            str.str("");
        }

    }

    TraceToOuput(str.str());
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool FastBoundingVolumeHierarchy::RayIntersection(const Ray& ray, double tMin, double tMax, HitRecord& record, size_t currentNodeIndex) const
{
    auto& node = m_nodes[currentNodeIndex];
    auto& objectList = *m_objectList;
    if (node.m_bounds.RayIntersection(ray, tMin, tMax))
    {
        if (node.indexToObject != std::numeric_limits<size_t>::max()) // leaf
        {
            return IntersectWithObjectType(objectList[node.indexToObject], ray, tMin, tMax, record);
        }
        else
        {
            bool hitLeft = RayIntersection(ray, tMin, tMax, record, node.m_leftIndex);
            bool hitRight = RayIntersection(ray, tMin, (hitLeft ? record.m_intersectionFactor : tMax), record, node.m_rightIndex);

            return hitLeft || hitRight;
        }
    }

    return false;
}
