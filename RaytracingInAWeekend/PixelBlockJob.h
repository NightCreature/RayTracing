#pragma once

#include "Math/vector4.h"
#include "Threading/Job.h"
#include "SceneObject.h"
#include "Timer.h"

#include <vector>


struct RenderOptions;

struct PixelBlockJobParameters
{
    std::vector<Vector4>* m_pixelArray = nullptr;
    RenderOptions* m_renderOptions = nullptr;
    std::vector<SceneObject>* scene = nullptr;
    size_t m_startIndex = 0;
    size_t m_endIndex = 0;
};

class PixelBlockJob : public Job
{
public:
    PixelBlockJob() = default;
    PixelBlockJob(const PixelBlockJobParameters& params) : m_params(params) {}
    virtual ~PixelBlockJob() {}

    virtual void Execute(size_t threadIndex) override;
private:
    PixelBlockJobParameters m_params;
    PerformanceTimer m_timer;
};

