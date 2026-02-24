
/*
	olc::PixelGameEngine3 Example - Custom Pixel Shaders (HEADLESS MODE)

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
#define OLC_USE_HEADLESS
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

	olc::gpu::OLC_SHADER_CLASS shaderExample;
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
			// 1) Horizontal Wobble
			vec2 samplePos = vec2(
				oTex.x + sin(oTex.y * frequency * pgeTargetSizeInPixels.y + pgeTotalTimeElapsed * 2.0) * amplitude, 
				oTex.y
			);



			// 2) Sobel Edge Detection

			// Sample 3x3 neighborhood for Sobel
			vec2 pixelSize = pgeInverseTargetSizeInPixels;
	
			float tl = length(texture(pgeTexture0, samplePos + vec2(-pixelSize.x, -pixelSize.y)).rgb);
			float t  = length(texture(pgeTexture0, samplePos + vec2(0.0, -pixelSize.y)).rgb);
			float tr = length(texture(pgeTexture0, samplePos + vec2(pixelSize.x, -pixelSize.y)).rgb);
			float l  = length(texture(pgeTexture0, samplePos + vec2(-pixelSize.x, 0.0)).rgb);
			float r  = length(texture(pgeTexture0, samplePos + vec2(pixelSize.x, 0.0)).rgb);
			float bl = length(texture(pgeTexture0, samplePos + vec2(-pixelSize.x, pixelSize.y)).rgb);
			float b  = length(texture(pgeTexture0, samplePos + vec2(0.0, pixelSize.y)).rgb);
			float br = length(texture(pgeTexture0, samplePos + vec2(pixelSize.x, pixelSize.y)).rgb);

			// Apply Sobel Kernels
			float sobelX = -tl + tr - 2.0 * l + 2.0 * r - bl + br;
			float sobelY = -tl - 2.0 * t - tr + bl + 2.0 * b + br;

			// Calculate edge magnitude
			float edge = sqrt(sobelX * sobelX + sobelY * sobelY);

			// 3) Colour bias vertically
			float bias = oTex.y;
			vec4 newCol = vec4(
				oCol.r * bias,
				oCol.g,
				oCol.b * (1.0 - bias),
				oCol.a
			);


			// 4) Combine effects		

			// pgeTexture0 is the primary texture unit 
			// pgeTexture1 is some other image
	
			vec4 texColour = texture(pgeTexture0, samplePos) * oCol;
			vec4 otherColour = texture(pgeTexture1, samplePos);

			vec4 edgeColour = vec4(newCol * edge) + otherColour * 0.5;
			pixel = vec4(edgeColour.rgb * texColour.a, texColour.a);
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
		CreateImage(imgWithoutFX, GetScreen().Size());

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
		draw.SetTarget(GetScreen());
		
		// Set the custom shader
		draw.SetShader(shaderExample);

		// Set custom parameters for the shader based 
		// on mouse position
		float frequency = (mouse.GetPosition().x / 256.0f) * 0.1f;
		float amplitude = (mouse.GetPosition().y / 240.0f) * 0.1f;
		draw.SetShaderUniform("frequency", frequency);
		draw.SetShaderUniform("amplitude", amplitude);

		// Our shader uses 2 texture units so we need to bind them here
		// By default, unit 0 is bound to the primary texture (imgWithoutFX)
		draw.SetShaderTexture(1, imgMini); // Bind imgMini to texture unit 1
				
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