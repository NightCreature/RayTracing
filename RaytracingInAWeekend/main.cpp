#include "BoundingVolumeHierarchy.h"
#include "DXWindow/GameWindow.h"
#include "DXWindow/StringHelperFunctions.h"
#include "MaterialHelpers.h"
#include "Math/vector3.h"
#include "PixelBlockJob.h"
#include "Ray.h"
#include "RenderOptions.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Threading/JobSystem.h"
#include "Timer.h"
#include "Types.h"
#include "Utilities.h"
#include <filesystem>
#include <iostream>
#include <limits>
#include <Optick.h>
#include <vector>

#include <windows.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")


constexpr size_t image_width = 1280;
constexpr size_t image_height = 720;
constexpr size_t nrSamples = 256;
constexpr size_t nrBounces = 25;
constexpr size_t numberOfWorkerThreads = 15;
constexpr size_t numberOfTasks = 90;

Vector3 LookAt(0, 1, 0);
Vector3 camPos(10, 1, 0);
Vector3 focusDistance = camPos - LookAt;

GameWindow window;
bool tracingDone = false;
std::filesystem::path fileName;
std::vector<Vector4> imageArray;

// Camera

RayTracingCamera cam(camPos, LookAt, Vector3::yAxis(), 20, image_width, image_height, 0.0, 1.0);
RenderOptions options = RenderOptions(cam, 90);

void OnKeyDown(WPARAM wParam)
{
    if (wParam == static_cast<std::underlying_type_t<Keys>>(Keys::KeyS)) //when "S" is pressed save the image
    {

        std::stringstream converter;
        auto fileNameTime = std::chrono::system_clock::now();
        converter << fileNameTime.time_since_epoch().count() << " " << options.m_outputWidth << "x" << options.m_outputHeight;
        auto path = std::filesystem::current_path();
        fileName = path / "Images" / converter.str();
        fileName.replace_extension(options.m_outputFileName);

        SaveImage(fileName.string(), imageArray, options);

        std::stringstream str;
        str << "Saved file at: " << fileName.string() << "\n";
        TraceToOuput(str.str());

    }

}

LRESULT CALLBACK MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_DESTROY)
    {
        PostQuitMessage(0);
    }
    else if (message == WM_PAINT)
    {
        if (tracingDone)
        {
            PAINTSTRUCT ps;
            HDC screen = BeginPaint(window.getWindowHandle(), &ps);   // Get the screen DC
            Gdiplus::Graphics graphics(screen);

            std::vector<MemoryBitmap::Color> pixels;
            convertVec4ToBGRBuffer(imageArray, pixels);

            BITMAPINFO bmi;
            memset(&bmi, 0, sizeof(bmi));
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = image_width;
            bmi.bmiHeader.biHeight = image_height;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biBitCount = 24;
            // Write pixels to 'data' however you want...
            Gdiplus::Bitmap myImage(&bmi, reinterpret_cast<char*>(&(pixels[0])));
            //Gdiplus::Image image(fileName.wstring().c_str());
            graphics.DrawImage(&myImage, 0, 0, image_width, image_height);

            EndPaint(window.getWindowHandle(), &ps);                  // clean up
        }
    }
    if (message == WM_KEYDOWN)
    {
        OnKeyDown(wParam);
    }
    if (message == WM_CLOSE)
    {
        PostQuitMessage(0);
    }
    //If the window doesn't exist yet pass on all the messages to the default win32 message handler
    return DefWindowProc(hwnd, message, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    //OPTICK_START_CAPTURE();
    
    //OPTICK_THREAD("MainThread");

    OPTICK_FRAME("Main");

    OPTICK_EVENT();
    // Image

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    //Window
    window.createWindow("RaytracerInAWeekend", "TraceMeSomeRays", image_width, image_height, &MessageHandler);
    window.showWindow();
   


    //Scene Setup
    std::vector<SceneObject> objectList;
    GenerateScene(objectList);

    BVHVisistStack stack;

    //BoundingVolumeHierarchy bvh(objectList);
    //options.m_boundingVolume = &bvh;
    //bvh.WalkBoudingVolume(stack);
    //options.m_boundingVolume = &bvh;

    //TraceToOuput("--------------------------------------------------------Array Based BVH--------------------------------------------------------\n");

    FastBoundingVolumeHierarchy fbhv(objectList);
    //fbhv.WalkBoudingVolumeBFS(stack, 0);
    
    options.m_fastBVH = &fbhv;

    HitRecord record;
    fbhv.RayIntersection(Ray(camPos, camPos - Vector3(0, 1, 0)), 0.001, std::numeric_limits<double>::infinity(), record, 0);
    //return 0;
    // Render
    imageArray.resize(image_width * image_height);

    //MT Setup

    options.m_numberOfWorkerThreads = numberOfWorkerThreads;
    options.m_numberOfTasks = numberOfTasks;
    options.m_numberOfBounces = nrBounces;
    options.m_numberOfSamples = nrSamples;
    options.m_outputHeight = image_height;
    options.m_outputWidth = image_width;
    options.m_aspectRatio = static_cast<double>(image_width) / image_height;
    options.m_outputFileName = ".bmp";

    JobSystem jobSystem(options.m_numberOfWorkerThreads);
    auto& jobQueue = jobSystem.GetJobQueue();

    PixelBlockJobParameters param;
    param.m_pixelArray = &imageArray;
    param.m_renderOptions = &options;
    param.scene = &objectList;

    size_t numberOfJobs = options.m_numberOfTasks;
    if (options.m_outputHeight / options.m_numberOfTasks != 0)
    {
        size_t pixelRowsPerJob = (options.m_outputHeight / options.m_numberOfTasks);
        size_t pixelRowsLeft = options.m_outputHeight - pixelRowsPerJob * numberOfJobs;
        size_t additionalJobs = static_cast<size_t>(std::ceil(pixelRowsLeft / static_cast<double>(pixelRowsPerJob)));
        numberOfJobs += additionalJobs;
    }
    std::vector<PixelBlockJob> pixelJobs;
    pixelJobs.resize(numberOfJobs);
    for (size_t counter = 0; counter < numberOfJobs; ++counter)
    {
        param.m_startIndex = param.m_endIndex;
        param.m_endIndex = param.m_startIndex + options.m_outputHeight / options.m_numberOfTasks;
        if (param.m_endIndex > options.m_outputHeight)
        {
            param.m_endIndex = options.m_outputHeight;
        }
        pixelJobs[counter] = PixelBlockJob(param);
    }

    for (auto& job : pixelJobs)
    {
        jobQueue.AddJob(&job);
    }


    //////////////////////////////////////////


    PerformanceTimer timer;
    timer.update();
    size_t now = timer.getTimeStamp();

    //SingleCoreTracing(objectList);
    jobSystem.WaitfForJobsToFinish();

    size_t end = timer.getTimeStamp();
    std::stringstream str;
    str << "Tracing took: " << ConvertTimeDurationToString((end - now) / timer.getResolution()).c_str();
    str << "Traced against: " << objectList.size() << " number of objects";
    TraceToOuput(str.str());

    double sampleScale = 1.0 / nrSamples;
    for (int j = 0; j < image_height; ++j)
    {
        for (int i = 0; i < image_width; ++i)
        {

            imageArray[image_width * j + i] *= sampleScale; //normalise the color values
        }
    }

    tracingDone = true;

    RedrawWindow(window.getWindowHandle(), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

    MSG message;
    message.message = WM_NULL;
    PeekMessage(&message, 0, 0, 0, PM_NOREMOVE);
    while (WM_QUIT != message.message)
    {

        message.message = WM_NULL;
        bool gotMessage = (PeekMessage(&message, 0, 0, 0, PM_NOREMOVE) != 0);
        if (gotMessage)
        {
            PeekMessage(&message, 0, 0, 0, PM_REMOVE);
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);

    //OPTICK_STOP_CAPTURE();

    std::stringstream converter;
    auto fileNameTime = std::chrono::system_clock::now();
    converter << fileNameTime.time_since_epoch().count() << " " << options.m_outputWidth << "x" << options.m_outputHeight;
    auto path = std::filesystem::current_path();
    auto profileCapturePath = path / "ProfileCaptures" / converter.str();
    profileCapturePath.replace_extension(".opt");
    //OPTICK_SAVE_CAPTURE(profileCapturePath.string().c_str());

   
    return 0;
}
