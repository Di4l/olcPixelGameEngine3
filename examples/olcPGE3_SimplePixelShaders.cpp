/*
	olc::PixelGameEngine3 Example - Simple Custom Pixel Shaders

	Example of three simple custom shaders:
		1. Greyscale (super simple)
		2. Negative (simple)
		3. Rainbow (still simple, but with a uniform defined)

	Licenced under the OLC-3 License
*/

#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

class Example_SimpleCustomShaders : public olc::PixelGameEngine
{
public:
	Example_SimpleCustomShaders() {}

	float fTotalTime{ 0.0f };
	olc::Image imgPixelFish;

	olc::gpu::Shader_GLSL33 customShader_GreyScale;
	olc::gpu::Shader_GLSL33 customShader_Negative;
	olc::gpu::Shader_GLSL33 customShader_Rainbow;

	const olc::vf2d TextPadding{ 4.0f, 4.0f };

	bool OnUserCreate() override
	{
		CreateImageFromFile(imgPixelFish, "./assets/pixelfish.png");

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		// Set up greyscale shader:

		std::string PS_GreyscaleMain =
			R"(
			void main()
			{
				vec4 texColor = texture(pgeTexture0, oTex) * oCol;
				float luminance = 0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b;
				vec4 texGreyscale = vec4(luminance, luminance, luminance, texColor.a);
				pixel = vec4(texGreyscale.rgb * texGreyscale.a, texGreyscale.a);
			}
			)";

			// Coefficients for luminance based on: https://en.wikipedia.org/wiki/Luma_(video)

		customShader_GreyScale.SetVertexShaderSource(olc::gpu::Shader::VS_DefaultHeader() + olc::gpu::Shader::VS_DefaultMain());
		customShader_GreyScale.SetPixelShaderSource(olc::gpu::Shader::PS_DefaultHeader() + PS_GreyscaleMain);

		// Check for shader compile errors
		std::string sResult_GreyScale = customShader_GreyScale.Compile();
		if (sResult_GreyScale != "OK")
		{
			std::cout << "Error compiling shader: " << sResult_GreyScale << std::endl;
			return false;
		}

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		// Set up negative shader:

		std::string PS_NegativeMain =
			R"(
			void main()
			{
				vec4 texColor = texture(pgeTexture0, oTex) * oCol;
				vec3 rgbNegative = 1.0 - texColor.rgb;
				rgbNegative = rgbNegative.bgr;
				rgbNegative += 0.15;
				vec4 texNegative = vec4(rgbNegative.r, rgbNegative.g, rgbNegative.b, texColor.a);
				pixel = vec4(texNegative.rgb * texNegative.a, texNegative.a);
			}
			)";

			// Shader based on a simple approximation of Balatro negative shader

		customShader_Negative.SetVertexShaderSource(olc::gpu::Shader::VS_DefaultHeader() + olc::gpu::Shader::VS_DefaultMain());
		customShader_Negative.SetPixelShaderSource(olc::gpu::Shader::PS_DefaultHeader() + PS_NegativeMain);

		// Check for shader compile errors
		std::string sResult_Negative = customShader_Negative.Compile();
		if (sResult_Negative != "OK")
		{
			std::cout << "Error compiling shader: " << sResult_Negative << std::endl;
			return false;
		}

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		// Set up rainbow shader:

		std::string PS_RainbowMain =
			R"(

			uniform float time;
			uniform float speed = 0.25;
			uniform float intensity = 0.3;

			void main()
			{
				vec4 texColor = texture(pgeTexture0, oTex) * oCol;
				
				float y = (oTex.x * 0.4 + oTex.y * 0.6) + speed * time;

				texColor.r *= (1.0 - intensity) + intensity * sin(y * 6.0 + 0.0);
				texColor.g *= (1.0 - intensity) + intensity * sin(y * 6.0 + 2.0 * 3.1415 / 3.0);
				texColor.b *= (1.0 - intensity) + intensity * sin(y * 6.0 + 2.0 * 3.1415 * 2.0 / 3.0);

				pixel = vec4(texColor.rgb * texColor.a, texColor.a);
			}
			)";

			// Simple rainbow shader - maths written out long hand for readability

		customShader_Rainbow.SetVertexShaderSource(olc::gpu::Shader::VS_DefaultHeader() + olc::gpu::Shader::VS_DefaultMain());
		customShader_Rainbow.SetPixelShaderSource(olc::gpu::Shader::PS_DefaultHeader() + PS_RainbowMain);

		// Check for shader compile errors
		std::string sResult_Rainbow = customShader_Rainbow.Compile();
		if (sResult_Rainbow != "OK")
		{
			std::cout << "Error compiling shader: " << sResult_Rainbow << std::endl;
			return false;
		}

		customShader_Rainbow.CreateUniform("time");

		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		fTotalTime += fElapsedTime;

		// Top left image - default shader:
		draw.ResetShader();
		olc::vf2d positionTL{ 0.0f, 0.0f };
		draw.Image(imgPixelFish, positionTL);
		draw.String(positionTL + TextPadding, "DEFAULT SHADER", olc::Colour::BLACK);

		// Top right image - greyscale shader:
		draw.SetShader(customShader_GreyScale);
		olc::vf2d positionTR{ 0.5f * GetScreen().Size().x, 0.0f };
		draw.Image(imgPixelFish, positionTR);
		draw.ResetShader();
		draw.String(positionTR + TextPadding, "GREYSCALE SHADER", olc::Colour::BLACK);

		// Bottom left image - negative shader:
		draw.SetShader(customShader_Negative);
		olc::vf2d positionBL{ 0.0f, 0.5f * GetScreen().Size().y};
		draw.Image(imgPixelFish, positionBL);
		draw.ResetShader();
		draw.String(positionBL + TextPadding, "NEGATIVE SHADER", olc::Colour::BLACK);

		// Bottom right image - rainbow shader:
		draw.SetShader(customShader_Rainbow);
		draw.SetShaderUniform("time", fTotalTime);
		olc::vf2d positionBR{ 0.5f * GetScreen().Size().x, 0.5f * GetScreen().Size().y };
		draw.Image(imgPixelFish, positionBR);
		draw.ResetShader();
		draw.String(positionBR + TextPadding, "RAINBOW SHADER", olc::Colour::BLACK);

		return true;
	}
};

int main()
{
	Example_SimpleCustomShaders simpleCustomShadersExample;
	if (simpleCustomShadersExample.Construct({ 320, 240 }, { 4, 4 }))
		simpleCustomShadersExample.Start();
	return 0;
}
