#include<nlohmann/json.hpp>
#include<spdlog/spdlog.h>
#include<LLGL/LLGL.h>

#include<optional>
#include<future>
#include<unordered_map>
#include<memory>

namespace LLGL
{
  class VKRenderSystem;
  class Shader;
}
namespace TYengine
{
    class ShaderComponent;

    class ShaderManager
    {
       public:
       LLGL::VKRenderSystem*vulkan_render_system;
       LLGL::RenderContext*context;
       std::shared_ptr<ShaderComponent> LoadOrCreateShader(std::string path);
       std::map<std::string,std::shared_ptr<ShaderComponent>>shader_map;
    };
};