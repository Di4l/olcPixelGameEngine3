#include "gpu_none.h"

//! START IMPLEMENTATION
namespace olc::gpu
{

    std::string Shader::static_PS_DefaultHeader;
    std::string Shader::static_PS_DefaultMain;
    std::string Shader::static_VS_DefaultHeader;
    std::string Shader::static_VS_DefaultMain;
    std::string Shader::static_GS_DefaultHeader;
    std::string Shader::static_GS_DefaultMain;		    

    std::string Shader_None::Compile()
    {
        return "OK";
    }

    int32_t Shader_None::CreateUniform(const std::string& name)
    {
        static int32_t uniformID = 0;
        mapUniforms.insert({ name, uniformID++ });
        return GetUniform(name);
    }

    // Constructs a GPU Device interface
    bool Renderer_None::CreateDevice(std::vector<void*> os_win_id, const RendererConfig& cfg)
    {
        return true;
    }

    // Destroys a GPU device interface
    bool Renderer_None::DestroyDevice()
    {
        return false;
    }

    // If applicable, relocate the rendering context
    bool Renderer_None::RetargetDevice(std::vector<void*> os_win_id)
    {
        return true;
    }

    // Prepare an OS rendering target
    bool Renderer_None::PrepareWindowTarget(std::vector<void*> os_win_id)
    {
        return true;
    }

    // Allocates a new texture resource in VRAM, returns handle
    uint32_t Renderer_None::CreateTexture(const olc::vi2d& vSize, const olc::ImageConfig& cfg)
    {
        static uint32_t id = 0;
        // deliberate post increment here to return 0 on the first call
        return static_cast<uint32_t>(id++);
    }

    // Writes to / updates an existing texture resource in VRAM, using existing Image in SRAM
    bool Renderer_None::WriteTexture(const uint32_t texid, olc::Image& image)
    {
        return true;
    }

    // Writes to / updates an existing Image in SRAM, from existing texture resource in VRAM
    bool Renderer_None::ReadTexture(const uint32_t texid, olc::Image& image)
    {
        return true;
    }

    // Destroys and releases texture resource for given handle
    bool Renderer_None::DeleteTexture(const uint32_t texid)
    {
        return true;
    }

    // Makes active the given texture resource (for subsequent sampling operations)
    bool Renderer_None::AssignTextureSource(const uint32_t slot, const uint32_t texid)
    {
        return true;
    }

    // Makes active the given texture resource (for subsequent rendering operations)
    bool Renderer_None::AssignTextureTarget(const uint32_t slot, const uint32_t texid)
    {
        return true;
    }

    // Resolves an MSAA texture into a normal texture
    bool Renderer_None::ResolveMSAA(const uint32_t msaaTexId)
    {
        return true;
    }

    // Change the shader used for subsequent GPU drawing tasks
    bool Renderer_None::ApplyShader(const Shader& shader)
    {
        return true;
    }

    // Reset to default shader for subsequent GPU drawing tasks
    bool Renderer_None::ApplyDefaultShader()
    {
        return ApplyShader(shaderDefault);
    }

    // Set uniform variable for subsequent GPU drawing tasks
    bool Renderer_None::SetUniform(const std::string& name, const float value)
    {
        return true;
    }

    // Set uniform variable for subsequent GPU drawing tasks
    bool Renderer_None::SetUniform(const std::string& name, const olc::vf2d& value)
    {
        return true;
    }

    // Set uniform variable for subsequent GPU drawing tasks
    bool Renderer_None::SetUniform(const std::string& name, const olc::Pixel value)
    {
        return true;
    }


    bool Renderer_None::DoGPUTask(const olc::GPUTask& task)
    {
        return true;
    }

    // Clears the viewport to a specific colour and depth
    bool Renderer_None::ClearViewport(const olc::Pixel col, bool bDepth, bool bStencil)
    {
        return true;
    }

    // Sets the viewport area of the drawing space
    bool Renderer_None::SetViewport(const olc::vf2d& pos, const olc::vf2d& size)
    {
        return true;
    }

    // Configures defaults prior to drawing
    bool Renderer_None::DisplayPrepare(const float fFrameElapsedTime, const float fTotalElapsedTime)
    {
        return true;
    }

    // Displays the final output
    bool Renderer_None::DisplayDraw(std::vector<void*> os_win_id, bool bVerticalSyncNow)
    {
        return true;
    }

}
//! END IMPLEMENTATION
