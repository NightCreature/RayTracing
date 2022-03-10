#include "MaterialHelpers.h"
#include "Utilities.h"

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool ScatterLambertian(const Ray& incidence, const HitRecord& hit, Color& color, Ray& scattered)
{
    auto scatterDirection = hit.m_normal + normalise(CreateRandomUnitVector());
    if (scatterDirection == Vector3::zero())
    {
        scatterDirection = hit.m_normal;
    }

    scattered = Ray(hit.m_point, scatterDirection);
    color = hit.m_material.m_albedo;

    return true;
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool ScatterMettalic(const Ray& incidence, const HitRecord& hit, Color& color, Ray& scattered)
{
    auto reflectedVector = Reflect(incidence, hit.m_normal);
    scattered = Ray(hit.m_point, reflectedVector + hit.m_material.m_fuzzyReflection * normalise(CreateRandomUnitVector()));
    color = hit.m_material.m_albedo;

    return reflectedVector.dot(hit.m_normal) > 0;
}

double Reflectance(double cosine, double refractionIndex)
{
    auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
bool ScatterDielectric(const Ray& incidence, const HitRecord& hit, Color& color, Ray& scattered)
{
    color = Vector3(1, 1, 1);
    double refractionRatio = hit.m_hitFrontFace ? 1.0 / hit.m_material.m_refractiveIndex : hit.m_material.m_refractiveIndex;
    Vector3 normalisedIncidence = normalise(incidence.m_direction);
    Vector3 invNormalisedIncidence = -normalisedIncidence;

    double cosTheta = std::min(invNormalisedIncidence.dot(hit.m_normal), 1.0);
    double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    bool cannotRefract = refractionRatio * sinTheta > 1.0;
    Vector3 direction;

    //Slicks thing
    double reflectance = Reflectance(cosTheta, refractionRatio);
    double randomVal = GetRandomValue();
    bool reflectancePass = reflectance > randomVal;

    //std::stringstream str;
    //str << "Reflectance: " << reflectance << " pass: " << reflectancePass  << " cosTheta: " << cosTheta << " randomVal: " << randomVal << "\n";
    //TraceToOuput(str.str());

    if (cannotRefract  || reflectancePass)
    {
        direction = Reflect(incidence, hit.m_normal);
    }
    else
    {
        //TraceToOuput("Refracting");
        direction = RefractInitial(normalisedIncidence, hit.m_normal, refractionRatio);
    }

    scattered = Ray(hit.m_point, direction);
    return true;
}
