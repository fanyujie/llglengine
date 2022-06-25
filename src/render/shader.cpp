#include"render/shader.hpp"

namespace TYengine
{
     void ShaderComponent::useShader(LLGL::CommandBuffer* command_buffer)
     {
         command_buffer->SetPipelineState(*pipeline_state);
         
     }
     
    void ShaderComponent::updateUniformBuffer(LLGL::CommandBuffer* command_buffer,LLGL::Buffer*buffer,std::string name,std::vector<float>data)
    {
        auto it = uniformtable.find(name);
        if(it==uniformtable.end())
        {
             spdlog::error("uniform name not found");
             return;
        }
        else
        {
            int offset = it->second.offset;
            int size = it->second.size;
            command_buffer->UpdateBuffer(*buffer,offset,(void*)data.data(),size);
        }
        
        //    command_buffer->UpdateBuffer(*buffer,)
    }
}