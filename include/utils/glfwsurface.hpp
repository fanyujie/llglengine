#pragma once
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>



#include<LLGL/LLGL.h>
#include<LLGL/Platform/NativeHandle.h>

class GlfwSurface : public LLGL::Surface
{
private:
    /* data */
public:
// Constructor and destructor
GlfwSurface (const LLGL :: Extent2D & size , const std :: string& title );
~ GlfwSurface ();
// Interface implementation
bool GetNativeHandle (void* nativeHandle, std::size_t nativeHandleSize) const override;
LLGL :: Extent2D GetContentSize () const override;
bool AdaptForVideoMode (LLGL :: VideoModeDescriptor & videoModeDesc ) override;
void ResetPixelFormat () override;
// Additional class functions
bool ProcessEvents();
std::unique_ptr<LLGL::Display> FindResidentDisplay() const override;

std :: string title_;
LLGL :: Extent2D size_;
GLFWwindow * wnd_ = nullptr; // GLFW window pointe
private:
GLFWwindow * CreateGLFWWindow ();
};

