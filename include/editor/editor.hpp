#pragma once
#include<spdlog/spdlog.h>
#include<LLGL/LLGL.h>



#include<optional>
#include<variant>
#include<string>
#include<memory>

#include<Renderer/Vulkan/VKPtr.h>
#include<vulkan/vulkan.h>

namespace LLGL
{
    class VKRenderContext;
    class VKRenderSystem;
    class VKRenderPass;
}
namespace ImGui 
{
    // struct ImGuiContext;
}
struct ImGuiContext;
struct GLFWwindow ;
// struct ImGui_ImplVulkanH_Window;
namespace TYengine
{
    class Editor
    {
    public:
        /* data */
        ~Editor();

    public:
        std::optional<std::variant<int,std::string>> initRender();
        void terminate();
        void uploadFronts();
        void frameRender();
        void initImguiWindow();
        std::unique_ptr<LLGL::RenderSystem> renderer;
        LLGL::RenderContextDescriptor contextDesc;
        LLGL::RenderContext* context;
        LLGL::VKRenderContext *vk_context;
        LLGL::Surface * window;
        LLGL::CommandQueue * command_queue;
        LLGL::CommandBuffer * command_buffer;
        GLFWwindow * glfw_window;
        ImGuiContext * imgui_context;
        LLGL::VKRenderSystem * vulkan_render_system;
        LLGL::VKPtr<VkDescriptorPool> descriptor_pool;
        const LLGL::VKRenderPass * editor_render_pass;
        // std::unique_ptr<ImGui_ImplVulkanH_Window>imgui_window;
        
    private :
        bool SwapChainRebuild=true;
    };
    
}


