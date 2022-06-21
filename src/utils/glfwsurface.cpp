#include"utils/glfwsurface.hpp"
#include<algorithm>
#include<iostream>
GlfwSurface::GlfwSurface(const LLGL::Extent2D & size , const std :: string& title):
title_ { title },
size_ { size },
wnd_ { CreateGLFWWindow () }
{
    // std::cout<<std::hex<<wnd_<<"window ptr \n";
    
}

GlfwSurface::~GlfwSurface()
{
// Destroy GLFW window
// glfwDestroyWindow (wnd_ );


}
GLFWwindow*GlfwSurface::CreateGLFWWindow ()
{
// Create GLFW window with class members
auto wnd = glfwCreateWindow (size_.width , size_.height , title_.c_str (), nullptr , nullptr );
if (! wnd)
throw std :: runtime_error ("failed to create GLFW window");
return wnd;
}
void GlfwSurface::ResetPixelFormat ()
{
// Destroy and recreate GLFW window
glfwDestroyWindow (wnd_ );
wnd_ = CreateGLFWWindow ();
}
bool GlfwSurface::GetNativeHandle (void* nativeHandle ,std::size_t nativeHandleSize) const
{
// This function must always return a valid native handle!
auto handle = reinterpret_cast <LLGL :: NativeHandle *>( nativeHandle );
handle ->window = glfwGetWin32Window (wnd_ );
// glfwhw
return true;
}
LLGL :: Extent2D GlfwSurface::GetContentSize () const
{
// Actually the client -area size of the window must be returned ,
// but for this example the entire window size is sufficient.
return size_;
}

bool GlfwSurface::AdaptForVideoMode (LLGL :: VideoModeDescriptor & videoModeDesc )
{
// Resize GLFW window for the new video mode resolution.
size_ = videoModeDesc . resolution ;
glfwSetWindowSize (wnd_ , size_.width , size_.height );
return true;
}

bool GlfwSurface::ProcessEvents ()
{
// Poll events from GLFW windowing system
glfwPollEvents ();
// Return true until the user pressed the close button
return ! glfwWindowShouldClose (wnd_ );
}

std::unique_ptr<LLGL::Display> GlfwSurface::FindResidentDisplay() const
{
      auto displayList = LLGL::Display::InstantiateList();
    
    int mxpos,mypos;
    int sizex,sizey;
    auto moniter=glfwGetPrimaryMonitor();
     glfwGetMonitorPos(moniter,&mxpos,&mypos);
     glfwGetMonitorPhysicalSize(moniter,&sizex,&sizey);
    int winArea  = static_cast<int>(sizex * sizey);

    for (auto& display : displayList)
    {
        auto offset = display->GetOffset();
        auto extent = display->GetDisplayMode().resolution;

        int scrX = static_cast<int>(extent.width);
        int scrY = static_cast<int>(extent.height);

        /* Calculate window boundaries relative to the current display */
        int x1 = mxpos - offset.x;
        int y1 = mypos - offset.y;
        int x2 = x1 + static_cast<int>(sizex);
        int y2 = y1 + static_cast<int>(sizey);

        /* Is window fully or partially inside the dispaly? */
        if (x2 >= 0 && x1 <= scrX &&
            y2 >= 0 && y1 <= scrY)
        {
            /* Is at least the half of the window inside the display? */
            x1 = max((int)0,x1);

            y1 = max(0,y1);

            x2 = min(x2-x1,scrX);
            y2 = min(y2-y1,scrY);

            auto visArea = x2 * y2;

            if (visArea * 2 >= winArea)
                return std::move(display);
        }
    }

    return nullptr;
}
