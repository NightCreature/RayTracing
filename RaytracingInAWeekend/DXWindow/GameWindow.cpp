#include "DXWindow/GameWindow.h"

#include <sstream>
#include "Utilities.h"
#include "StringHelperFunctions.h"

bool GameWindow::m_destroyable = false;

///-----------------------------------------------------------------------------
///! @brief  Constructor GameWindow
///! @remark
///-----------------------------------------------------------------------------
GameWindow::GameWindow()
{
    m_destroyable = false;
    m_fps = 0;
    m_framesCounter = 0;
    m_timeAmount = 0.0f;
    m_windowWidth = CW_USEDEFAULT;
    m_windowHeight = CW_USEDEFAULT;
    m_hasFocus = false;
}

///-----------------------------------------------------------------------------
///! @brief  Setup a win32 window
///! @remark
///-----------------------------------------------------------------------------
bool GameWindow::createWindow( const std::string& applicationName, const std::string windowTitle, size_t windowWidth, size_t windowHeight, WNDPROC messageHandler )
{
    std::wstring appName;
    std::wstring winTitle;
    convertToWideString(applicationName, appName);
    convertToWideString(windowTitle, winTitle);

    m_hInstance = ( HINSTANCE )GetModuleHandle( NULL );
    m_windowClassExtended.cbSize        = sizeof(WNDCLASSEX);
    m_windowClassExtended.style         = CS_CLASSDC;
    m_windowClassExtended.lpfnWndProc   = messageHandler;
    m_windowClassExtended.cbClsExtra    = 0;
    m_windowClassExtended.cbWndExtra    = 0;
    m_windowClassExtended.hInstance     = m_hInstance;
    m_windowClassExtended.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    m_windowClassExtended.hCursor       = LoadCursor (NULL, IDC_ARROW);
    m_windowClassExtended.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    m_windowClassExtended.lpszMenuName  = NULL;
    m_windowClassExtended.lpszClassName = appName.c_str();
    m_windowClassExtended.hIconSm       = m_windowClassExtended.hIcon;

    if (!RegisterClassEx(&m_windowClassExtended))
    {
        TraceToOuput("failed to register the window class!");
        return false;
    }
    
    m_windowWidth = windowWidth + 16;
    m_windowHeight = windowHeight + 32 + 2; //add window title height
    //const ISetting<int>* widthSetting = SettingsManager::getInstance().getSetting<int>("WindowWidth");
    //if (widthSetting)
    //{
    //    m_windowWidth = widthSetting->getData();     
    //}
    //const ISetting<int>* heightSetting = SettingsManager::getInstance().getSetting<int>("WindowHeight");
    //if (heightSetting)
    //{
    //    m_windowHeight = heightSetting->getData();     
    //}

    m_windowHandle = CreateWindowEx (WS_EX_OVERLAPPEDWINDOW,
        appName.c_str(), 
        winTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        m_windowWidth, 
        m_windowHeight, 
        NULL, 
        NULL, 
        m_windowClassExtended.hInstance,
        NULL);

    if (m_windowHandle == NULL)
    {
        std::stringstream str;
        str << "FAILED to create a window with error code: " << GetLastError() << "\n";
        TraceToOuput(str.str());
        return false;
    }

    m_windowTitle = windowTitle;

    //TraceToOuput("Window successfully created");

    return true;
}

///-----------------------------------------------------------------------------
///! @brief  Show the window that was created
///! @remark
///-----------------------------------------------------------------------------
void GameWindow::showWindow()
{
    ShowWindow   (m_windowHandle, SW_SHOW);
    UpdateWindow (m_windowHandle);
}

///-----------------------------------------------------------------------------
///! @brief   TODO enter a description
///! @remark
///-----------------------------------------------------------------------------
void GameWindow::update( float elapsedTime, double time )
{
    calculateFPS(elapsedTime);
    
    time = 0.0;
}

///-----------------------------------------------------------------------------
///! @brief Calculate the frames per second
///-----------------------------------------------------------------------------
void GameWindow::calculateFPS(float elapsedTime)
{
    if (m_timeAmount < 1.0f)
    {
        ++m_framesCounter;
        m_timeAmount += elapsedTime;
    }
    else
    {
        m_fps = m_framesCounter;
        setFpsInWindowTitle(elapsedTime);
        m_framesCounter = 0;
        m_timeAmount = 0.0f;
    }
}

///-----------------------------------------------------------------------------
///! @brief   Change the window title to include the fps
///-----------------------------------------------------------------------------
void GameWindow::setFpsInWindowTitle(float elpasedTime)
{
    std::wstringstream strStream;
    strStream << m_windowTitle.c_str() << " - fps: " << m_fps << " frame time: " << elpasedTime * 1000 << "ms";
    SetWindowText(m_windowHandle, strStream.str().c_str());
}

///-----------------------------------------------------------------------------
///! @brief   TODO enter a description
///! @remark
///-----------------------------------------------------------------------------
void GameWindow::initialise()
{
}

///-----------------------------------------------------------------------------
///! @brief   TODO enter a description
///! @remark
///-----------------------------------------------------------------------------
void GameWindow::messageHandler( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if (message == WM_CLOSE)
    {
        PostQuitMessage(0);
    }

    lParam = 0;
}