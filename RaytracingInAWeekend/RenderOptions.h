#pragma once

#include "Camera.h"
#include "Math/matrix44.h"

#include <cmath>
#include <filesystem>
#include <string>
#include "Ray.h"
#include "Scene.h"

struct BoundingVolumeHierarchy;
struct FastBoundingVolumeHierarchy;

struct RenderOptions
{
    RenderOptions(double fov)
        : m_aspectRatio(static_cast<double>(m_outputWidth) / static_cast<double>(m_outputHeight))
        , m_fov(fov)
        , m_tanFov(tan(fov/2))
    {}

    void Deserialise(const std::filesystem::path& settingsFilePath);

    std::string m_outputFileName;

    Scene m_scene;
    Matrix44 m_projectionMatrix;
    Matrix44 m_invProjection;

    size_t m_outputWidth = 256;
    size_t m_outputHeight= 256;
    size_t m_numberOfSamples;
    size_t m_numberOfBounces = 1;
    size_t m_numberOfWorkerThreads = 1;
    size_t m_numberOfTasks = 1;
    double m_aspectRatio;
    double m_fov;
    double m_tanFov;
    double m_nearPlaneDistance = 0.1;

    bool m_usePathTracing = false;
    bool m_useHeapAllocatedBvH = true;
};
