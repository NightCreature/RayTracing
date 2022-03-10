#include "Utilities.h"

#include "RenderOptions.h"

#include "Math/MathUtilityFunctions.h"
#include "Math/matrix33.h"
#include "Math/vector3.h"
#include "Ray.h"

#include <random>
#include <vector>
#include <windows.h>
#include <string>
#include <random>
#include "DXWindow/StringHelperFunctions.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
void convertVec4ToBGRBuffer(const std::vector<Vector4>& pixels, std::vector<MemoryBitmap::Color>& outPixels)
{
    outPixels.resize(pixels.size());
    // convert from RGB F32 to BGR U8
    for (size_t counter = 0; counter < pixels.size(); ++counter)
    {
        const Vector4& src = pixels[counter];
        MemoryBitmap::Color& dest = outPixels[counter];

        // apply gamma correction
        Vector4 correctedPixel(pow(src.x(), 1.0 / 2.2), pow(src.y(), 1.0 / 2.2), pow(src.z(), 1.0 / 2.2), 1.0);

        // clamp and convert
        dest.r = uint8_t(math::clamp(correctedPixel.x() * 255.0, 0.0, 255.0));
        dest.g = uint8_t(math::clamp(correctedPixel.y() * 255.0, 0.0, 255.0));
        dest.b = uint8_t(math::clamp(correctedPixel.z() * 255.0, 0.0, 255.0));
    }
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
MemoryBitmap createBitmapInMemory(const std::vector<Vector4>& pixels, const std::string& fileName, const RenderOptions& renderOptions)
{
    MemoryBitmap retVal;

    // allocate memory for our bitmap BGR U8 image
    std::vector<MemoryBitmap::Color>& outPixels = retVal.pixels;
    convertVec4ToBGRBuffer(pixels, outPixels);

    // make the header info
    retVal.header.bfType = 0x4D42;
    retVal.header.bfReserved1 = 0;
    retVal.header.bfReserved2 = 0;
    retVal.header.bfOffBits = 54;

    retVal.infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    retVal.infoHeader.biWidth = (LONG)(renderOptions.m_outputWidth);
    retVal.infoHeader.biHeight = (LONG)(renderOptions.m_outputHeight);
    retVal.infoHeader.biPlanes = 1;
    retVal.infoHeader.biBitCount = 24;
    retVal.infoHeader.biCompression = 0;
    retVal.infoHeader.biSizeImage = (DWORD)(outPixels.size() * 3);
    retVal.infoHeader.biXPelsPerMeter = 0;
    retVal.infoHeader.biYPelsPerMeter = 0;
    retVal.infoHeader.biClrUsed = 0;
    retVal.infoHeader.biClrImportant = 0;

    retVal.header.bfSize = retVal.infoHeader.biSizeImage + retVal.header.bfOffBits;

    return retVal;
}

///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
bool SaveImage(const std::string& fileName, std::vector<Vector4>& pixels, const RenderOptions& renderOptions)
{
    MemoryBitmap bitMap = createBitmapInMemory(pixels, fileName, renderOptions);

    // open the file if we can
    FILE* file;
    fopen_s(&file, fileName.c_str(), "wb");
    if (!file)
        return false;


    // write the data and close the file
    fwrite(&bitMap.header, sizeof(bitMap.header), 1, file);
    fwrite(&bitMap.infoHeader, sizeof(bitMap.infoHeader), 1, file);
    //std::reverse(outPixels.begin(), outPixels.end());
    fwrite(&bitMap.pixels[0], bitMap.infoHeader.biSizeImage, 1, file);
    fclose(file);
    return true;
}

std::random_device srd;
std::mt19937 sgen(srd());
std::uniform_real_distribution<> sdis(-1.0, 1.0);

///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
Vector3 CreateRandomUnitVector()
{
    Vector3 dir = Vector3(sdis(sgen), sdis(sgen), sdis(sgen));
    while (dir.dot(dir) >= 1)
    {
        dir = Vector3(sdis(sgen), sdis(sgen), sdis(sgen));
    }

    return dir;
}

///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
Vector3 CreateRandomVectorInUnitDisk()
{
    Vector3 dir = Vector3(GetRandomValueInRange(-1, 1), GetRandomValueInRange(-1, 1), 0);
    while (dir.dot(dir) >= 1)
    {
        dir = Vector3(GetRandomValueInRange(-1, 1), GetRandomValueInRange(-1, 1), 0);
    }

    return dir;
}

Matrix33 GetTangentSpace(Vector3 normal)
{
    // Choose a helper vector for the cross product
    Vector3 helper = Vector3(1, 0, 0);
    if (abs(normal.x()) > 0.99)
        helper = Vector3(0, 0, 1);
    // Generate vectors
    Vector3 tangent = cross(normal, helper);
    tangent.normalize();
    Vector3 binormal = cross(normal, tangent);
    binormal.normalize();
    return Matrix33(tangent, binormal, normal);
}

///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
Vector3 CreateHemiSphereRandomVector(const Vector3& normal)
{
    // Uniformly sample hemisphere direction
    double cosTheta = sdis(sgen);
    double sinTheta = sqrt(std::max(0.0, 1.0 - cosTheta * cosTheta));
    double phi = 2 * math::gmPI * sdis(sgen);
    Vector3 tangentSpaceDir = Vector3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
    // Transform direction to world space
    auto tangentSpace = GetTangentSpace(normal);
    return multiply(tangentSpaceDir, tangentSpace);
}

Vector3 CreateRandomVectorInHemiSphere(const Vector3& normal)
{
    auto direction = normalise(CreateRandomUnitVector());
    if (direction.dot(normal) > 0.0)
    {
        return direction;
    }
    
    return -direction;
}

///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
Vector3 Reflect(const Ray& ray, const Vector3& normal)
{
    auto dir = normalise(ray.m_direction);
    return dir - 2.f * dir.dot(normal) * normal;
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
Vector3 RefractInitial(const Vector3& normalisedIncidence, const Vector3& normal, double etai_over_etat)
{
    auto uv = -normalisedIncidence;
    auto cos_theta = std::min(uv.dot(normal), 1.0);
    Vector3 rOutPerpendicular = etai_over_etat * (normalisedIncidence + cos_theta * normal);
    Vector3 rOutParrallel = -std::sqrt(fabs(1.0 - rOutPerpendicular.dot(rOutPerpendicular))) * normal;
    return rOutPerpendicular + rOutParrallel;
}

///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
Vector3 Refract(const Ray& ray, const Vector3& normal, double refractionIndex )
{
    Vector3 dir;
    double cosi = math::clamp(ray.m_direction.dot(normal), -1.0, 1.0);
    double etai = 1;
    double etat = refractionIndex;
    Vector3 Normal = normal;
    if (cosi < 0)
    {
        cosi = -cosi;
    }
    else
    {
        std::swap(etai, etat);
        Normal = -normal;
    }

    double eta = etai / etat;
    double k = 1 - eta * eta * (1 - cosi * cosi);
    if (k > 0)
    {
        dir = eta * ray.m_direction + (eta * cosi - sqrt(k)) * Normal;
    }                        
    
    return dir;
}

///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
void Fresnel(const Ray& ray, const Vector3& normal, double refractionIndex, double& fresnelFactor)
{
    double cosi = math::clamp(ray.m_direction.dot(normal), -1.0, 1.0);
    double etai = 1;
    double etat = refractionIndex;
    if (cosi > 0) 
    {
        std::swap(etai, etat); 
    }
    // Compute sini using Snell's law
    double sint = etai / etat * sqrt(std::max(0., 1 - cosi * cosi));
    // Total internal reflection
    if (sint >= 1) {
        fresnelFactor = 1;
    }
    else {
        double cost = sqrt(std::max(0., 1 - sint * sint));
        cosi = fabs(cosi);
        double Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        double Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        fresnelFactor = (Rs * Rs + Rp * Rp) / 2;
    }
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
}

///-----------------------------------------------------------------------------
///! @brief 
///! @remark
///-----------------------------------------------------------------------------
std::string ConvertTimeDurationToString(double timerDuration)
{
    std::stringstream str("");
    auto timeSpentDouble = 0.0;
    auto lessThenSeconds = modf(timerDuration, &timeSpentDouble);
    lessThenSeconds *= 10000000000;
    auto timeSpent = static_cast<size_t>(timeSpentDouble);
    auto seconds = timeSpent % 60;
    timeSpent /= 60;
    auto minutes = timeSpent % 60;
    timeSpent /= 60;
    auto hours = timeSpent % 24;
    auto days = timeSpent / 24;
    str << days << " days and " << hours << ":" << minutes << ":" << seconds << "." << static_cast<size_t>(lessThenSeconds) << "s\n";
    return str.str();
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
void TraceToOuput(const std::string& str)
{
    std::cout << str;
    OutputDebugStringA(str.c_str());
}

///-----------------------------------------------------------------------------
///! @brief   gets value between 0 and 1
///! @remark
///-----------------------------------------------------------------------------
double GetRandomValue()
{
    return dis(gen);
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
double GetRandomValueInRange(double min, double max)
{
    return min + (max - min) * GetRandomValue();
}

///-----------------------------------------------------------------------------
///! @brief   
///! @remark
///-----------------------------------------------------------------------------
size_t GetRandomUInt(size_t min /*= 0*/, size_t max /*= 1*/)
{
    return static_cast<size_t>(GetRandomValueInRange(min, max + 1));
}