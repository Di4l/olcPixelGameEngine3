
/*
	olc::PixelGameEngine3 Example - Custom Pixel Shaders

	Draws a few lines using different styles and techniques, AND THEN
	renders THE ENTIRE SCENE USING A CUSTOM PIXEL SHADER!

	Licenced under the OLC-3 License
*/

/*
	NOTE! This is an advanced example demonstrating custom pixel shaders.

	It assumes you are familiar with olcPixelGameEngine3 and GLSL shader
	programming. If you are new to either of these topics, please refer 
	to the other examples and documentation first.
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating a custom pixel shader effect. 
// This class overrides the olc::PixelGameEngine base class by 
// implementing the OnUserCreate() and OnUserUpdate() functions
class Example_CustomPixelShader : public olc::PixelGameEngine
{
public:
	Example_CustomPixelShader()
	{

	}

protected:
	// We accumulate total time for some animation
	float fTotalTime = 0.0f;

	olc::gpu::Shader_GLSL33 shaderExample;
	olc::Image imgWithoutFX;

	olc::Image imgMini;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{

		// Custom pixel shader main() function and uniforms
		std::string custom_ps_main =
		R"(
		uniform float frequency; 
		uniform float amplitude;

		void main()
		{
			// Horizontal Wobble
			vec2 sample = vec2(
				oTex.x + sin(oTex.y * frequency * pgeTargetSizeInPixels.y + pgeTotalTimeElapsed * 2.0) * amplitude, 
				oTex.y
			);

			// Colour bias vertically
			float bias = oTex.y;
			vec4 newCol = vec4(
				oCol.r * bias,
				oCol.g,
				oCol.b * (1.0 - bias),
				oCol.a
			);

			vec4 texColor = texture(pgeTexture, sample) * newCol;
			pixel = vec4(texColor.rgb * texColor.a, texColor.a);
		}
		)";


		// Construct pixel shader from default header, but use custom main()
		shaderExample.SetPixelShaderSource(
			olc::gpu::Shader::PS_DefaultHeader() +
			custom_ps_main);

		// Just use the PGE3 default vertex shader
		shaderExample.SetVertexShaderSource(
			olc::gpu::Shader::VS_DefaultHeader() + 
			olc::gpu::Shader::VS_DefaultMain()
		);

		// Check for shader compile errors
		std::string sResult = shaderExample.Compile();
		if (sResult != "OK")
		{
			std::cout << "Error compiling shader: " << sResult << std::endl;
			return false;
		}

		// Add custom uniforms used by custom shader. The "system"
		// will have taken care of required PGE3 uniforms
		shaderExample.CreateUniform("frequency");
		shaderExample.CreateUniform("amplitude");

		// Create off-screen image to draw to
		CreateImage(imgWithoutFX, GetDefaultImage().Size());

		// Load a small image to draw for fun
		CreateImageFromFile(imgMini, "./assets/minsanity_texture.png");
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Set rendering target to off-screen image, we will draw to this
		// entirely normally, then apply a custom shader when we draw the
		// off-screen image to the screen
		draw.SetTarget(imgWithoutFX);

		// === Stolen from olcPGE3_Lines.cpp ==

		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Simple white line
		draw.Line({ 10.0f, 10.0f }, { 246.0f, 40.0f }, olc::Colour::WHITE);

		// Gradient line
		draw.Line({ 10.0f, 50.0f }, olc::Colour::RED, { 246.0f, 80.0f }, olc::Colour::YELLOW);

		// Rotating gradient lines
		fTotalTime += fElapsedTime;
		olc::vf2d p1 = { 64.0f, 160.0f };
		olc::vf2d p2 = olc::vf2d{ std::cos(fTotalTime), std::sin(fTotalTime) } * 64.0f;

		draw.Line(p1, olc::Colour::CYAN, p1 + p2, olc::Colour::MAGENTA);
		draw.Line(p1, olc::Colour::CYAN, p1 - p2, olc::Colour::MAGENTA);
		draw.Line(p1, olc::Colour::CYAN, p1 + olc::vf2d{ -p2.y, p2.x }, olc::Colour::MAGENTA);
		draw.Line(p1, olc::Colour::CYAN, p1 + olc::vf2d{ p2.y, -p2.x }, olc::Colour::MAGENTA);

		// Line to mouse pointer with tint
		float alpha = (std::sin(fTotalTime * 4.0f) + 1.0f) / 2.0f;
		draw.Line(p1, mouse.GetPosition(), olc::Colour::WHITE, olc::PixelF(1.0f, 0.0f, 0.0f, alpha));

		// Boundary box (with fixed	coordinates)
		draw.Line({ 0.0f, 0.0f }, { 255.0f, 0.0f }, olc::Colour::DARK_GREY);
		draw.Line({ 255.0f, 0.0f }, { 255.0f, 239.0f }, olc::Colour::DARK_GREY);
		draw.Line({ 255.0f, 239.0f }, { 0.0f, 239.0f }, olc::Colour::DARK_GREY);
		draw.Line({ 0.0f, 239.0f }, { 0.0f, 0.0f }, olc::Colour::DARK_GREY);
		// === End stolen code ===

		// Draws an image for the fun of it
		draw.Image(imgMini, mouse.GetPosition());

		// Copy image to screen with new shader
		draw.SetTarget(GetDefaultImage());
		
		// Set the custom shader
		draw.SetShader(shaderExample);

		// Set custom parameters for the shader based 
		// on mouse position
		float frequency = (mouse.GetPosition().x / 256.0f) * 0.1f;
		float amplitude = (mouse.GetPosition().y / 240.0f) * 0.1f;
		draw.SetShaderUniform("frequency", frequency);
		draw.SetShaderUniform("amplitude", amplitude);
				
		// Present image with shader effect - this is required
		// because here is where the shader is actually applied
		draw.Image(imgWithoutFX, { 0, 0 });

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_CustomPixelShader demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}