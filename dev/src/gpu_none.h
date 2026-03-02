#pragma once

#include "gpu_iface.h"

//! START DECLARATION

#if !defined(PGE_RENDERER_NONE_DECLARED)
namespace olc::gpu
{
    class Shader_None : public olc::gpu::Shader
    {
    public:
        std::string Compile() override;
        int32_t CreateUniform(const std::string& name) override;
    };
    
    class Renderer_None : public olc::gpu::Renderer
    {
    public: // Device Stuff
        // Constructs a GPU Device interface
        bool CreateDevice(std::vector<void*> os_win_id, const RendererConfig& cfg) override;
        // Destroys a GPU device interface
        bool DestroyDevice() override;
        // If applicable, relocate the rendering context
        bool RetargetDevice(std::vector<void*> os_win_id) override;
        // Prepare an OS rendering target
        bool PrepareWindowTarget(std::vector<void*> os_win_id) override;


    public: // Texture Resource Stuff
        // Allocates a new texture resource in VRAM, returns handle
        uint32_t CreateTexture(const olc::vi2d& vSize, const olc::ImageConfig& cfg = olc::ImageConfig()) override;
        // Writes to / updates an existing texture resource in VRAM, using existing Image in SRAM
        bool WriteTexture(const uint32_t texid, olc::Image& image) override;
        // Writes to / updates an existing Image in SRAM, from existing texture resource in VRAM
        bool ReadTexture(const uint32_t texid, olc::Image& image) override;
        // Destroys and releases texture resource for given handle
        bool DeleteTexture(const uint32_t texid) override;
        // Makes active the given texture resource (for subsequent sampling operations)
        bool AssignTextureSource(const uint32_t slot, const uint32_t texid) override;
        // Makes active the given texture resource (for subsequent rendering operations)
        bool AssignTextureTarget(const uint32_t slot, const uint32_t texid) override;
        // Resolves an MSAA texture into a normal texture
        bool ResolveMSAA(const uint32_t msaaTexId) override;

    public: // Shader Construction Stuff
        // Change the shader used for subsequent GPU drawing tasks
        bool ApplyShader(const Shader& shader) override;
        // Reset to default shader for subsequent GPU drawing tasks
        bool ApplyDefaultShader() override;
        // Set uniform variable for subsequent GPU drawing tasks
        bool SetUniform(const std::string& name, const float value) override;
        // Set uniform variable for subsequent GPU drawing tasks
        bool SetUniform(const std::string& name, const olc::vf2d& value) override;
        // Set uniform variable for subsequent GPU drawing tasks
        bool SetUniform(const std::string& name, const olc::Pixel value) override;

    public: // GPU Task Stuff
        bool DoGPUTask(const olc::GPUTask& task) override;

    public: // Swap Chain Stuff
        // Clears the viewport to a specific colour and depth
        bool ClearViewport(const olc::Pixel col, bool bDepth, bool bStencil) override;
        // Sets the viewport area of the drawing space
        bool SetViewport(const olc::vf2d& pos, const olc::vf2d& size) override;
        // Configures defaults prior to drawing
        bool DisplayPrepare(const float fFrameElapsedTime, const float fTotalElapsedTime) override;
        // Displays the final output
        bool DisplayDraw(std::vector<void*> os_win_id, bool bVerticalSyncNow) override;
    protected:
        Shader_None shaderDefault;
    };
}



#define PGE_RENDERER_NONE_DECLARED 1
#endif
//! END DECLARATION