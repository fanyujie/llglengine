cmake_minimum_required(VERSION 3.2)
project(llglengine)
set(CMAKE_CXX_STANDARD 17)


# set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# subproject config
set (All_headers ${CMAKE_SOURCE_DIR}/include)

include_directories(${All_headers})
set (Editor_src ${CMAKE_SOURCE_DIR}/src/editor)
set (Utils_src ${CMAKE_SOURCE_DIR}/src/utils)
set (Render_src ${CMAKE_SOURCE_DIR}/src/render)
# set ()

# supress warning
if(WIN32)
  add_definitions("/wd4251")
endif(WIN32)

#vulkan 
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    include_directories(${Vulkan_INCLUDE_DIR})
    # target_link_directories(hello PUBLIC ./thirdparty/LLGL/build/Debug)
else()
    message("Missing Vulkan -> LLGL_Vulkan renderer will be excluded from project")
endif()


#LLGLD_LIB
set(LLGLD_header ./thirdparty/LLGL/include ./thirdparty/LLGL/sourcesinclude)
set(LLGLD_lib_path ./thirdparty/LLGL/build/Debug)
set(LLGLD_libs LLGL_VulkanD.lib LLGLD.lib ${Vulkan_LIBRARY})

macro(ImportLLGLD target_name)
target_include_directories(${target_name} PUBLIC ${LLGLD_header})
target_link_directories(${target_name}  PUBLIC ${LLGLD_lib_path})
target_link_libraries(${target_name} ${LLGLD_libs})
endmacro(ImportLLGLD)



#spdlogD_LIB
set(spdlog_header ./thirdparty/spdlog/include)
set(spdlog_lib_path ./thirdparty/spdlog/build/Debug)
set(spdlog_libs spdlogd.lib)

macro(ImportspdlogD target_name)
target_compile_definitions(${target_name} PUBLIC SPDLOG_COMPILED_LIB)
target_include_directories(${target_name} PUBLIC ${spdlog_header})
target_link_directories(${target_name} PUBLIC ${spdlog_lib_path})
target_link_libraries(${target_name} ${spdlog_libs})
endmacro(ImportspdlogD)



#glfw_LIB
set(glfw_header ./thirdparty/glfw/include)
set(glfw_lib_path ./thirdparty/glfw/build/Release)
set(glfw_libs glfw3.lib)
macro(Importglfw target_name)
target_include_directories(${target_name} PUBLIC ${glfw_header})
target_link_directories(${target_name} PUBLIC ${glfw_lib_path})
# if(NOT EXISTS ${glfw_lib_path}/glfw3.lib)
# message("lib not exist ")
# endif()

target_link_libraries(${target_name} ${glfw_libs})

endmacro(Importglfw )

#imgui_LIB
set(imgui_header ./thirdparty/imgui/include ./thirdparty/imgui/include/imgui)
set(imgui_src ./thirdparty/imgui/src)

file(GLOB imgui_cpps ${imgui_src}/*.cpp)
add_library(imgui STATIC ${imgui_cpps})
target_include_directories(imgui PUBLIC ${imgui_header})
Importglfw(imgui)

macro(Importimgui target_name)
target_include_directories(${target_name} PUBLIC ${imgui_header})
# Importglfw(${target_name})
target_link_libraries(${target_name} imgui)
endmacro(Importimgui)




# jsonlib
set(nljson_header ./thirdparty/nljson/single_include)


macro(Importnljson target_name)
target_precompile_headers(${target_name} PUBLIC ${nljson_header}/nlohmann/json.hpp)
target_include_directories(${target_name} PUBLIC ${nljson_header})
endmacro(Importnljson)




set(BUILD_EDITOR 1)
#utils_lib
file(GLOB utils_cpps ${Utils_src}/*.cpp)
message("utils_cpps ${utils_cpps}")
add_library(utils STATIC ${utils_cpps})

ImportLLGLD(utils)

Importglfw(utils)

macro(Importutils target_name)
# ImportLLGLD(${target_name})
# Importglfw(${target_name})
target_link_libraries(${target_name} utils)
endmacro(Importutils )



#render_lib
file(GLOB render_cpps ${Render_src}/*.cpp)
add_library(render STATIC ${render_cpps})
    #LLGL
ImportLLGLD(render)
    #SPDLOG
ImportspdlogD(render)
    #utils
Importutils(render)
    #json
Importnljson(render)
macro(Importrender target_name)
target_compile_definitions(${target_name} PUBLIC LLGL_ENABLE_UTILITY=1)
target_include_directories(${target_name} PUBLIC ./include)
target_link_libraries(${target_name} render)

endmacro(Importrender)




#Editor_lib 
file(GLOB editor_cpps ${Editor_src}/*.cpp)

add_library(editor STATIC ${editor_cpps})
    #LLGL
ImportLLGLD(editor)
    #SPDLOG
ImportspdlogD(editor)
    #imgui
Importimgui(editor)
    #utils
Importutils(editor)
    # target_include_directories(editor PUBLIC ${Editor_header})
Importnljson(editor)
macro(Importeditor target_name)
# target_include_directories(${target_name} PUBLIC ${Editor_header})
# ImportLLGLD(${target_name})
# ImportspdlogD(${target_name})
# Importglfw(${target_name})
# Importimgui(${target_name})
# Importutils(${target_name})
target_link_libraries(${target_name} editor)
endmacro(Importeditor)
    
    


add_executable(hello hello.cpp)
# LLGL
ImportLLGLD(hello)
# SPDLOG
ImportspdlogD(hello)
#
Importutils(hello)
#
# Importimgui(hello)
# editor 
Importeditor(hello)


# add_executable(example ./src/demo/Example.cpp)
# ImportLLGLD(example)


