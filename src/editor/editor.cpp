#include"editor/editor.hpp"
// #include"include/editor/editor.hpp"
#include<Renderer/Vulkan/VKRenderContext.h>
#include<Renderer/Vulkan/VKRenderSystem.h>
#include<Renderer/Vulkan/VKCommandBuffer.h>
#include<vulkan/vulkan.h>
#include"utils/glfwsurface.hpp"
#include<imgui/imgui.h>
#include<imgui/imgui_impl_vulkan.h>
#include<imgui/imgui_impl_glfw.h>

// #include"imgui.h"
// #include"imgui_impl_glfw.h"
// #include"imgui_impl_vulkan.h"
#include<LLGL/Platform/NativeHandle.h>

namespace TYengine
{
    static void check_vk_result(VkResult err)
    {
    if (err == 0)
        return;
        // fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        spdlog::error("[vulkan] Error: VkResult = {}",(int)err);
    }
    std::optional<std::variant<int,std::string>> Editor::initRender()
    {
            spdlog::info("Welcome to spdlog!");

            spdlog::info("finding modules...............");

            for(auto&name : LLGL::RenderSystem::FindModules())
            {
                spdlog::info("detect module {}",name);
            }


          LLGL::RendererConfigurationVulkan RendererConfigurationVulkan{};
          char*appname="Vulkan";
         auto&appconfig= RendererConfigurationVulkan.application;
         appconfig.applicationName="Vulkan";
         appconfig.applicationVersion=VK_MAKE_VERSION(1, 0, 0);
         appconfig.engineName="LLGL";
         appconfig.engineVersion=VK_MAKE_VERSION(1, 0, 0);

        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        extensions.push_back("VK_LAYER_LUNARG_standard_validation");
        extensions.push_back("VK_LAYER_RENDERDOC_Capture");
        
        for (auto & extension :extensions)
        {
            RendererConfigurationVulkan.enabledLayers.push_back(std::string(extension));
        }

          LLGL::RenderSystemDescriptor renderSystemDescriptor{};
          renderSystemDescriptor.rendererConfig=&RendererConfigurationVulkan;
          renderSystemDescriptor.moduleName="Vulkan";
          renderSystemDescriptor.rendererConfigSize=sizeof(RendererConfigurationVulkan);

          this->renderer=LLGL::RenderSystem::Load(renderSystemDescriptor);

          

        LLGL::RenderContextDescriptor contextDesc;
        {
            contextDesc.videoMode.resolution    = { 1600, 1200 };
            contextDesc.vsync.enabled           = true;
            #ifdef ENABLE_MULTISAMPLING
            contextDesc.samples                 = 8; // check if LLGL adapts sample count that is too high
            #endif
        }
        glfwInit();
        glfwSetErrorCallback(
        []( int error, const char * msg )
        {
            // std::cerr << "glfw: "
            //             << "(" << error << ") " << msg << std::endl;
            spdlog::error("glfw : ({}) : {}",error,msg);
        } );
                
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); 
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        this->vulkan_render_system= dynamic_cast<LLGL::VKRenderSystem*>(this->renderer.get());

        if(vulkan_render_system==nullptr)
        {
            spdlog::error("cast to vulkan_rendersystem fail!");
        }
        this->context=this->renderer->CreateRenderContext(contextDesc,std::make_shared<GlfwSurface>(contextDesc.videoMode.resolution ,"TEST EDITOR"));

        this->vk_context=dynamic_cast<LLGL::VKRenderContext*>(this->context);
        if(this->vk_context==nullptr)
        {
            spdlog::error("cast to vulkan fail!");
        }
        else
        {
            spdlog::info("cast to vulkan success");
            VkDevice devicehandle= (this->vk_context->device_).Get();
            spdlog::info("vulkan cast info {}",(uint64_t)devicehandle);
        }
        const auto& info = renderer->GetRendererInfo();
        
        spdlog::info("render:{},device:{},vendor:{},shaderlang:{}",
        info.rendererName,info.deviceName,info.vendorName,info.shadingLanguageName);


        this->window=&(this->context->GetSurface());
        

        
        // this->context->GetSurface().

        
        

        this->command_buffer=this->renderer->CreateCommandBuffer();
        this->command_queue=this->renderer->GetCommandQueue();

        // command_buffer->UpdateBuffer()
        // imgui
        IMGUI_CHECKVERSION();
        this->imgui_context=ImGui::CreateContext();//this->imgui_context=
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        
        glfw_window=(dynamic_cast<GlfwSurface*>(this->window))->wnd_;

        spdlog::info("window:{}",(void*)glfw_window);
        if(!ImGui_ImplGlfw_InitForVulkan(glfw_window,true))
        {
            spdlog::error("imgui init glfw window fail!");
            
        }


        // check debug layer;
        spdlog::info("check debug layer state : {}",this->vulkan_render_system->debugLayerEnabled_);
        // imgui shader
        // descriptor 
        
        LLGL::VKPtr<VkDescriptorPool>::Copyfromto(LLGL::VKPtr<VkDescriptorPool>(vk_context->device_,vkDestroyDescriptorPool),this->descriptor_pool);
         VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        
        VkResult erra;
        if((erra=vkCreateDescriptorPool(vk_context->device_,&pool_info,nullptr,descriptor_pool.ReleaseAndGetAddressOf()))!=VK_SUCCESS)
        {
            spdlog::error("creat imgui descriptorpool fail code : {}",(int)erra);

        }





        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = vk_context->instance_;
        init_info.PhysicalDevice = vk_context->physicalDevice_;
        init_info.Device = vk_context->device_;
        init_info.QueueFamily = vulkan_render_system->device_.GetQueueFamilyIndices().graphicsFamily;
        init_info.Queue = vulkan_render_system->device_.GetVkQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool =  descriptor_pool;
        init_info.Subpass = 0;
        init_info.MinImageCount = vk_context->numSwapChainBuffers_;
        init_info.ImageCount = vk_context->g_maxNumColorBuffers;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = check_vk_result;
            
        
        // ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);

        this->editor_render_pass=dynamic_cast<const LLGL::VKRenderPass*>(vk_context->GetRenderPass());
        
        if(editor_render_pass==nullptr)
        {
            spdlog::error("editor_render_pass castfail!");
        }
        if(!ImGui_ImplVulkan_Init(&init_info,editor_render_pass->GetVkRenderPass()))
        {
            spdlog::error("ImGui_ImplVulkan_Init_fail!");
        }
        //uploadfronts;
        uploadFronts();
        bool show_demo_window=true,show_another_window=true;




    
    while (window->ProcessEvents())           {
        
            /* code */
        if (SwapChainRebuild)
        {
            int width, height;
            glfwGetFramebufferSize(this->glfw_window, &width, &height);
            if (width > 0 && height > 0)
            {
                
                ImGui_ImplVulkan_SetMinImageCount(vk_context->numSwapChainBuffers_);
                // ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
                // g_MainWindowData.FrameIndex = 0;
                // g_SwapChainRebuild = false;
          
                
            
            }
            Sleep(10);
        }
        

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);



        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // ImGui Rendering
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        this->command_buffer->Begin();
        {
           auto vk_buffer=static_cast<LLGL::VKCommandBuffer*>(command_buffer);
           this->command_buffer->BeginRenderPass(*context);
           this->command_buffer->Clear(LLGL::ClearFlags::Color);
            ImGui_ImplVulkan_RenderDrawData(draw_data,vk_buffer->GetVkCommandBuffer());

            
            this->command_buffer->EndRenderPass();
        }
        this->command_buffer->End();
        this->command_queue->Submit(*command_buffer);

        

        context->Present();
        
        }
        
        return {};
    }
    void Editor::terminate()
    {
        //  vkDestroyDescriptorPool(vk_context->device_,descriptor_pool,nullptr);

        descriptor_pool.Release();


        spdlog::info("editor terminating.......");
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(glfw_window);
        glfwTerminate();
        
        spdlog::info("editor terminated");
    }

    void Editor::uploadFronts()
    {
        command_buffer->Begin();
        {
            auto vk_buffer=static_cast<LLGL::VKCommandBuffer*>(command_buffer);
            if(!ImGui_ImplVulkan_CreateFontsTexture(vk_buffer->GetVkCommandBuffer()))
            {
                spdlog::error("ImGui_ImplVulkan_CreateFontsTexture fail!");
            }

        }
        command_buffer->End();
        command_queue->Submit(*command_buffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
    void Editor::initImguiWindow()
    {
        // imgui_window=std::make_unique<ImGui_ImplVulkanH_Window>();
        // imgui_window->Surface=vk_context->surface_;
        // imgui_window->SurfaceFormat=vk_context->swapChainFormat_;



    }
    Editor::~Editor()
    {
       
        
    }
    
}