#pragma once

#include "Types.h"

enum class MaterialType : size_t
{
    Lambertian,
    Metallic,
    Dielectric,
    Count
};

struct Material
{
    Material() {}
    Material(const Color& albedo, MaterialType type, double fuzzFactor = 0.0, double refractiveIndex = 1.0) : m_albedo(albedo), m_type(type), m_fuzzyReflection(fuzzFactor), m_refractiveIndex(refractiveIndex) {}
  
    Color m_albedo;
    Color m_emitted;

    MaterialType m_type = MaterialType::Lambertian;
    double m_fuzzyReflection = 0.0;
    double m_refractiveIndex = 1.0;
};