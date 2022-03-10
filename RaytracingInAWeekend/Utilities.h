#pragma once

#include "Math/MathUtilityFunctions.h"
#include "Math/vector4.h"
#include "Math/vector3.h"

#include <stdint.h>
#include <string>
#include <vector>
#include <windows.h>

struct Ray;
struct RenderOptions;
class Vector3;

struct MemoryBitmap
{
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER infoHeader;
    
    struct Color
    {
        uint8_t b, g, r;
    };

    std::vector<Color> pixels;
};

void convertVec4ToBGRBuffer(const std::vector<Vector4>& pixels, std::vector<MemoryBitmap::Color>& outPixels);
MemoryBitmap createBitmapInMemory(const std::vector<Vector4>& pixels, const std::string& fileName, const RenderOptions& renderOptions);
bool SaveImage(const std::string& fileName, std::vector<Vector4>& pixels, const RenderOptions& renderOptions);

Vector3 CreateRandomUnitVector();
Vector3 CreateRandomVectorInUnitDisk();
Vector3 CreateHemiSphereRandomVector(const Vector3& normal);
Vector3 CreateRandomVectorInHemiSphere(const Vector3& normal);
Vector3 Reflect(const Ray& ray, const Vector3& normal);
Vector3 RefractInitial(const Vector3& normalisedIncidence, const Vector3& normal, double etai_over_etat);
Vector3 Refract(const Ray& ray, const Vector3& normal, double refractionIndex);
void Fresnel(const Ray& ray, const Vector3& normal, double refractionIndex, double& fresnelFactor);

///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
std::string ConvertTimeDurationToString(double timerDuration);

void TraceToOuput(const std::string& str);


double GetRandomValue();
double GetRandomValueInRange(double min, double max);
size_t GetRandomUInt(size_t min = 0, size_t max = 1);
