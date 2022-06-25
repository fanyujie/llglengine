#include<render/shadermanager.hpp>
#include<LLGL/Shader.h>
#include<Renderer/Vulkan/VKRenderSystem.h>
#define LLGL_ENABLE_UTILITY
#include<LLGL/Utility.h>
#include<fstream>
#include<render/shader.hpp>
namespace TYengine
{
    std::map<std::string,LLGL::ShaderType>strtotype{
        {"Vert",LLGL::ShaderType::Vertex},
        {"Frag",LLGL::ShaderType::Fragment},
        {"Comp",LLGL::ShaderType::Compute},
        {"Geom",LLGL::ShaderType::Geometry},

    };
    std::map<std::string,LLGL::Format>strtoformat
    {
        {"vec2",LLGL::Format::RG32Float},
        {"vec3",LLGL::Format::RGB32Float},
        {"vec4",LLGL::Format::RGBA32Float},
        {"float",LLGL::Format::R32Float},
        {"int",LLGL::Format::R32SInt},
        {"uint",LLGL::Format::R32UInt},
        {"mat4",LLGL::Format::RGBA32Float},
        {"mat3",LLGL::Format::RGB32Float},
        {"mat2",LLGL::Format::RG32Float},
       
    };
    std::map<std::string,int>strtoformatsize
    {
        {"vec2",2},
        {"vec3",3},
        {"vec4",4},
        {"float",1},
        {"int",1},
        {"uint",1},
        {"mat4",4},
        {"mat3",3},
        {"mat2",2},
    };
    std::shared_ptr<ShaderComponent> ShaderManager::LoadOrCreateShader(std::string path)
    {
        auto it = shader_map.find(path);
        if(it != shader_map.end())
        {
            return it->second;
        }
        else
        {
            std::ifstream file(path+"/config.json");
            if(!file.is_open())
            {
                spdlog::error("[ShaderManager] Failed to open shader file: {}",path);
                return nullptr;
            }
            nlohmann::json json;
            file >> json;
            file.close();

            std::map<LLGL::ShaderType,LLGL::Shader*>shader_stages;
            LLGL::VertexFormat vertexFormat;
            for(auto layout :json["layouts"])
            {
                // LLGL::ShaderDescriptor desc;
                //shadertype
                auto shaderstage =strtotype[layout["stage"].get<std::string>()];
 
                auto shadername = json["shadername"].get<std::string>();
                auto shaderDesc = LLGL::ShaderDescFromFile(shaderstage, (path+"/"+layout["filename"].get<std::string>()).c_str());
                // layout_descs.push_back(shaderDesc);


                shader_stages[shaderstage] = vulkan_render_system->CreateShader(shaderDesc);
                if(shader_stages[shaderstage] == nullptr)
                {
                    spdlog::error("[ShaderManager] Failed to create shader: {}",path);
                    return nullptr;
                }

                LLGL::BufferDescriptor vertexBufferDesc;
                if(shaderstage==LLGL::ShaderType::Vertex)
                {
                    int stride=0;
                    
                    for(auto vertexformat:layout["layouts"])
                    {
                        auto name = vertexformat["name"].get<std::string>();
                        auto type = vertexformat["type"].get<std::string>();
                        auto location = vertexformat["location"].get<int>();
                        int size=strtoformatsize[type];
                        stride+=(size*sizeof(float));
                        vertexFormat.AppendAttribute({ name.c_str(), strtoformat[type] });
                    }
                    vertexFormat.SetStride(stride);

                }
                
            }
                    // Create shader program which is used as composite
            LLGL::ShaderProgramDescriptor shaderProgramDesc;
            for (auto[shaderType, shader] : shader_stages)
            {
                switch (shaderType)
                {
                    case LLGL::ShaderType::Vertex:
                    shaderProgramDesc.vertexShader = shader;
                    break;
                    case LLGL::ShaderType::Fragment:
                    shaderProgramDesc.fragmentShader = shader;
                    break;
                    case LLGL::ShaderType::Geometry:
                    shaderProgramDesc.geometryShader = shader;
                    break;
                    case LLGL::ShaderType::Compute:
                    shaderProgramDesc.computeShader = shader;
                    break;
                

                  
                }
            }
            auto shaderprogram=vulkan_render_system->CreateShaderProgram(shaderProgramDesc);
            if(shaderprogram==nullptr)
            {
                spdlog::error("[ShaderManager] Failed to create shader program: {}",path);
                return nullptr;
            }
            //pipeline
            LLGL::GraphicsPipelineDescriptor pipelineDesc;
            {
                pipelineDesc.shaderProgram                  = shaderprogram;
                pipelineDesc.renderPass                     = context->GetRenderPass();
                #ifdef ENABLE_MULTISAMPLING
                pipelineDesc.rasterizer.multiSampleEnabled  = (contextDesc.samples > 1);
                #endif
            }
            auto pipeline = vulkan_render_system->CreatePipelineState(pipelineDesc);
            if(pipeline==nullptr)
            {
                spdlog::error("[ShaderManager] Failed to create pipeline: {}",path);
                return nullptr;
            }
            auto shadercomponent=std::make_shared<ShaderComponent>(shaderprogram,pipeline,vertexFormat);

        
            
            // auto shader = vulkan_render_system->CreateShader()
            shader_map[path] = shadercomponent;
            return shader_map[path];
        }
    }
}