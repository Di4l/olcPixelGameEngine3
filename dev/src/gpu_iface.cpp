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
        return mapUniforms.at(name);
    }

    int32_t gpu::Shader::GetShaderID() const
    {
        return nCompiledShaderID;
    }
}

//! END IMPLEMENTATION