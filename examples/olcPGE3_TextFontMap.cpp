/*
	olc::PixelGameEngine3 Example - Custom Fonts (basic)

	Shows how to create and use a custom font, from a font map

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating a custom font. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_FontMap: public olc::PixelGameEngine
{
public:
	Example_FontMap()
	{

	}

protected:

	// The Font!
	olc::Font fontExample;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Load an image from file that contains the characters we can render
		CreateImageFromFile(fontExample.imgFont, "./assets/olc_font.png");

		// Fill the whole character set, we'll only use a subset in reality
		// but it makes the glyph lookup very quick and easy
		fontExample.glyphs.resize(256, olc::FontGlyph{fontExample.imgFont});

		// For convenience the image has the characters laid out
		// in a grid resembling their ASCII values.
		// 
		//   0x|0123456789ABCDEF
		// ----+----------------
		// 0x20| !"#$%&'()*+,-./
		// 0x30|0123456789:;<=>?
		// 0x40|@ABCDEFGHIJKLMNO
		// 0x50|PQRSTUVWXYZ[\]^_
		// 0x60|`abcdefghijklmno
		// 0x70|pqrstuvwxyz{|}~
		//
		// Note 1: 0x7F is reserved for the OLC "::" namespace character

		// Note 2: You do not have to arrange your font map like this, as
		// you can use the region() function of olc::Image to extract from
		// arbitrary locations in the image. This arrangement is just for
		// convenience in this example.

		// Note 3: Currently, all your font characters must come from the
		// same image, as Draw2D::String() uses batch rendering internally.
		// However, the FontGlyph::imgGlyph member can reference any region
		// from any image, so you could roll-your-own DrawString() function!

		// Note 4: Don't forget all olc::Images are writeable, so you could
		// even have the font change dynamically during runtime!

		// Note 5: Let's be honest, glyphs dont even have to be text characters,
		// you could use this system to draw icons or other things! Fill ya boots...

		// Note 6: Non-English characters can easily be supported now too. However
		// the direct 256 look-up may be restrictive. I may consider changing the
		// glyph vector to a map in future versions.

		// Note 7: This is a lot of notes...

		// Each source character is 10x10 pixels in size. This does not have
		// to be the case, but it is for this font map - and makes this example
		// easier to understand
		olc::vf2d vCharSize = { 10,10 };

		// Define common line height for the font. We deliberately
		// reduce the height by 1 pixel, because thats how the OLC font
		// is supposed to look. The chartacters are deliberately 
		// crushed into each other by one pixel in +x and +y
		fontExample.fLineHeight = vCharSize.y - 1;

		// Manually define each glyph in the font		
		for (char c = ' '; c <= '~'; c++)
		{
			// Calculate position in font image
			olc::vf2d vCharPos = olc::vf2d((c - 32) % 16, (c - 32) / 16);

			// Extract glyph image from font image
			fontExample.glyphs[c].imgGlyph = fontExample.imgFont.region(vCharPos * vCharSize, vCharSize);
			
			// All characters have zero lead in spacing in this font
			fontExample.glyphs[c].spacing = 0.0f;

			// There is no distinction between proportional and monospace
			// for the OLC font, so we set them both to the same value, 
			// including the deliberate 1 pixel crush
			fontExample.glyphs[c].vPropSize = { vCharSize.x - 1, vCharSize.y-1 };
			fontExample.glyphs[c].vMonoSize = { vCharSize.x - 1, vCharSize.y-1 };
		}

		// Special case the OLC "::" namespace character at 0x7F
		fontExample.glyphs[0x7F].vMonoSize = { vCharSize.x - 1, vCharSize.y - 1 };
		fontExample.glyphs[0x7F].vPropSize = { vCharSize.x - 1, vCharSize.y - 1 };
		fontExample.glyphs[0x7F].spacing = 0.0f;
		fontExample.glyphs[0x7F].imgGlyph = fontExample.imgFont.region(olc::vf2d(15.0f, 5.0f) * vCharSize, vCharSize);
		
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::GREY);

		std::string sExample = 
R"(Hello World!

This is a test of the OLC
Font System. As you can see,
we are sourcing our glyphs
from a custom image file.

This allows for great
flexibility in design and
style of text rendering!

"Hello" - @javidx9, 2026
)";

		// Draw the text above
		draw.String({ 4, 4 }, sExample, olc::Colour::WHITE, { 1,1 }, fontExample);

		// Proportional font drawing can be used - this custom font doesn't care
		draw.StringProp({ 4,140 }, "olc\x7FPixelGameEngine v3.0", olc::Colour::WHITE, {1,1}, fontExample);
					  
		// Draw all characters in the font set using different colours
		draw.String({ 4,160 }, " !\"#$%&'()*+,-./", olc::Colour::RED, { 1,1 }, fontExample);
		draw.String({ 4,170 }, "0123456789:;<=>?", olc::Colour::TANGERINE, { 1,1 }, fontExample);
		draw.String({ 4,180 }, "@ABCDEFGHIJKLMNO", olc::Colour::YELLOW, { 1,1 }, fontExample);
		draw.String({ 4,190 }, "PQRSTUVWXYZ[\\]^_", olc::Colour::MAGENTA, { 1,1 }, fontExample);
		draw.String({ 4,200 }, "`abcdefghijklmno", olc::Colour::GREEN, { 1,1 }, fontExample);
		draw.String({ 4,210 }, "pqrstuvwxyz{|}~\x7F", olc::Colour::BLUE, { 1,1 }, fontExample);

		// Draw a fancy logo
		draw.FilledRoundedRect({ 183, 155 }, { 66, 66 }, 5, olc::Colour::TANGERINE);
		draw.String({ 188, 160 }, "olc\npge\nIII", olc::Colour::WHITE, { 2,2 }, fontExample);


		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_FontMap demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}