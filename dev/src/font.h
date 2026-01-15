#pragma once

//! START STDHEADER GLOBAL
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <iostream>
//! END STDHEADER


//! START CUSTOMHEADER GLOBAL
#include "config.h"
#include "image.h"
#include "imload_iface.h"
//! END CUSTOMHEADER


//! START DECLARATION
#if !defined(PGE_FONT_DECLARED)
namespace olc
{
	class PGEWindow;

	struct FontGlyph	
	{
		// Region of an olc::Image that contains the glyph
		// in bitmap format. Typically WHITE on TRANSPARENT...
		// ...though have fun with crazy alternatives :P
		olc::ImageRegion imgGlyph;
		// Leading spacing in pixels before glyph
		float spacing;
		// Size of the glyph in proportional format
		olc::vf2d vPropSize;
		// Size of the glyph in monospace format
		olc::vf2d vMonoSize;

	};
	

	class Font
	{
	public:
		Font() = default;
		~Font() = default;

	public:		
		// Image that contains all glyphs. We store it here for
		// for batch rendering, which does restrict all glyphs
		// to being from the same image (for now)
		olc::Image imgFont;
		// All glyphs in the font (may use a map in future for non-ASCII)
		std::vector<FontGlyph> glyphs;
		// Line height in pixels
		float fLineHeight = 10.0f;
		// Tab width in pixels
		float fTabWidth = 32.0f;
	};		

	// Predefined classic PGE font
	inline olc::Font fontClassicPGE;

	namespace pgeguts
	{
		// Create the classic PGE font
		void CreateClassicFont(olc::PGEWindow* pge);
	}

}

#define PGE_FONT_DECLARED 1
#endif
//! END DECLARATION