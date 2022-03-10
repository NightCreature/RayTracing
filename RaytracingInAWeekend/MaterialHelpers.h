#pragma once
#include "HitRecord.h"
#include "Types.h"

struct Ray;

bool ScatterLambertian(const Ray& incidence, const HitRecord& hit, Color& color, Ray& scattered);
bool ScatterMettalic(const Ray& incidence, const HitRecord& hit, Color& color, Ray& scattered);
bool ScatterDielectric(const Ray& incidence, const HitRecord& hit, Color& color, Ray& scattered);

inline bool ScatterMaterialType(const Ray& incidence, const HitRecord& hit, Color& color, Ray& scattered)
{
    switch (hit.m_material.m_type)
    {
    case MaterialType::Lambertian:
        return ScatterLambertian(incidence, hit, color, scattered);
    case MaterialType::Metallic:
        return ScatterMettalic(incidence, hit, color, scattered);
    case MaterialType::Dielectric:
        return ScatterDielectric(incidence, hit, color, scattered);
    case MaterialType::Count:
    default:
        break;
    }

    return false;
}
