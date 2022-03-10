#include "Scene.h"


///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
void GenerateScene(std::vector<SceneObject>& objectList)
{
    Material dielectric(Color(1, 1, 1), MaterialType::Dielectric, 0.0, 1.5);
    Material metal(Color(1), MaterialType::Metallic, 0.0);
    Material lambertian(Color(0.4, 0.2, 0.1), MaterialType::Lambertian);
    Material groundMaterial(Color(0.5, 0.5, 0.5), MaterialType::Lambertian);
    //objectList.emplace_back(SceneObject(Vector3(0, -1000, 0), Vector3(1000), ObjectType::Sphere, groundMaterial));

    for (int a = -5; a < 5; a++)
    {
        for (int b = -5; b < 5; b++)
        {
            auto choose_mat = GetRandomValue();
            Vector3 center(a + 0.9 * GetRandomValue(), 0.2, b + 0.9 * GetRandomValue());

            if ((center - Vector3(4, 0.2, 0)).length() > 0.9)
            {
                Material mat;

                if (choose_mat < 0.8) 
                {
                    // diffuse
                    mat.m_type = MaterialType::Lambertian;
                    mat.m_albedo = Color(GetRandomValue(), GetRandomValue(), GetRandomValue()) * Color(GetRandomValue(), GetRandomValue(), GetRandomValue());
                }
                else if (choose_mat < 0.95) 
                {
                    // metal
                    mat.m_type = MaterialType::Metallic;
                    mat.m_albedo = Color(GetRandomValueInRange(0.5, 1), GetRandomValueInRange(0.5, 1), GetRandomValueInRange(0.5, 1));
                    mat.m_fuzzyReflection = GetRandomValueInRange(0, 0.5);
                }
                else if (choose_mat < 0.98)
                {
                    // glass
                    mat = dielectric;
                }
                else
                {
                    //This is a light
                    mat.m_emitted = Color(200); //Strong white light
                    TraceToOuput("Added a light!!!!\n");
                }

                objectList.emplace_back(SceneObject(center, Vector3(0.2), ObjectType::Sphere, mat));
            }
        }
    }

    objectList.emplace_back(SceneObject(Vector3(0, 1, 0), Vector3(1), ObjectType::Sphere, metal));
    objectList.emplace_back(SceneObject(Vector3(-4, 1, 0), Vector3(1), ObjectType::Sphere, lambertian));
    objectList.emplace_back(SceneObject(Vector3(4, 1, 0), Vector3(1), ObjectType::Sphere, dielectric));

    //objectList.push_back(SceneObject(Vector3(0, -100.5, -1), Vector3(100), ObjectType::Sphere, Material(Color(0.8, 0.8, 0.0), MaterialType::Lambertian)));
    //objectList.push_back(SceneObject(Vector3(0, 0, -1), Vector3(0.5), ObjectType::Sphere, Material(Vector3(1.1, 1.2, 5.5), MaterialType::Lambertian)));
    //objectList.push_back(SceneObject(Vector3(-1, 0, -1), Vector3(0.5), ObjectType::Sphere, Material(Vector3(0.8, 0.8, 0.8), MaterialType::Dielectric, 0, 1.5)));
    //objectList.push_back(SceneObject(Vector3(-1, 0, -1), Vector3(-0.4), ObjectType::Sphere, Material(Vector3(0.8, 0.8, 0.8), MaterialType::Dielectric, 0, 1.5)));
    //objectList.push_back(SceneObject(Vector3(1, 0, -1), Vector3(0.5), ObjectType::Sphere, Material(Vector3(0.8, 0.6, 0.2), MaterialType::Metallic, 0.00)));
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
void GenerateBoudingBoxList(const std::vector<SceneObject>& objectList, std::vector<AABoundingBox>& boundingBoxList)
{
    for (const auto& object : objectList)
    {
        boundingBoxList.emplace_back(CreateBoundingBoxForObjectType(object));
    }
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
AABoundingBox GenerateBoundingBoxForList(const std::vector<SceneObject>& objectList)
{
    AABoundingBox retVal; //This is an invalid sized box (min: inf, max: -inf) and if you combine this with another box the other box is always your bound

    for (const auto& object : objectList)
    {
        retVal = AABoundingBox(retVal, CreateBoundingBoxForObjectType(object));
    }

    return retVal;
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
void Scene::CreateScene()
{
    GenerateScene(m_scene);
}
