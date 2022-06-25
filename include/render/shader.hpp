#pragma once
#include<nlohmann/json.hpp>
#include<spdlog/spdlog.h>
#include<LLGL/LLGL.h>
#include<LLGL/Shader.h>
#include<optional>
#include<future>
#include<unordered_map>
namespace TYengine
{
      struct UniformMemory
       {
              int offset;
              int size;
        /* data */
       };
    class ShaderComponent
    {
       public:
       LLGL::ShaderProgram* shader_program;
       LLGL::PipelineState* pipeline_state;
       LLGL::VertexFormat vertexFormat;
       //todo uniform buffer
       std::vector<float>host_buffer;
     
       
       std::map<std::string,UniformMemory>uniformtable;
       ShaderComponent(LLGL::ShaderProgram* shader_program,LLGL::PipelineState* pipeline_state,LLGL::VertexFormat vertexFormat)
       {
           this->shader_program = shader_program;
           this->pipeline_state = pipeline_state;
           this->vertexFormat = vertexFormat;
       }
       void useShader(LLGL::CommandBuffer* command_buffer);
       /* 
       data
       
        */
       void updateUniformBuffer(LLGL::CommandBuffer* command_buffer,LLGL::Buffer*buffer,std::string name,std::vector<float>data);
       

       
    //    std::unordered_map<>
    };
    
}
