#include "gpu_iface.h"


//! START IMPLEMENTATION
namespace olc
{
    gpu::Shader::~Shader()
    {
    }

    void gpu::Shader::SetPixelShaderSource(const std::string& src)
    {
        srcPixelShader = src;
    }

    void gpu::Shader::SetVertexShaderSource(const std::string& src)
    {
        srcVertexShader = src;
    }

    void gpu::Shader::SetGeometryShaderSource(const std::string& src)
    {
        srcGeometryShader = src;
    }

    int32_t gpu::Shader::GetUniform(const std::string& name) const
    {
        if (mapUniforms.contains(name))
            return int32_t(mapUniforms.at(name));
        else
            return -1;
    }

    uint32_t gpu::Shader::GetShaderID() const
    {
        return nCompiledShaderID;
    }

    std::string gpu::Shader::PS_DefaultHeader() 
    {
        return static_PS_DefaultHeader;
    }

    std::string gpu::Shader::PS_DefaultMain() 
    {
		return static_PS_DefaultMain;
    }

    std::string gpu::Shader::VS_DefaultHeader() 
    {
		return static_VS_DefaultHeader;        
    }

    std::string gpu::Shader::VS_DefaultMain() 
    {
		return static_VS_DefaultMain;      
    }

    std::string gpu::Shader::GS_DefaultHeader() 
    {
		return static_GS_DefaultHeader;
    }

    std::string gpu::Shader::GS_DefaultMain() 
    {
		return static_GS_DefaultMain;
    }
}

//! END IMPLEMENTATION