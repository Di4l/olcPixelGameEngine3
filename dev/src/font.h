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
		olc::ImageRegion imgGlyph;
		olc::vf2d vKerning;
	};

	class Font
	{
	public:
		Font() = default;
		~Font() = default;

		//void CreateFontSheet(const olc::Image& imgFont, const std::array<FontGlyph, 256>& glyphs);
		//bool LoadFontSheet(imload::ImageLoader* imload, const std::string& sImageFile, const std::array<FontGlyph, 256>& glyphs);
		//FontGlyph GetGlyph(const char c);

	public:		
		olc::Image imgFontSheet;
		std::vector<FontGlyph> glyphs;
		olc::vf2d vCharSize;
	};		

	inline olc::Font fontClassicPGE;

	namespace pgeguts
	{
		void CreateClassicFont(olc::PGEWindow* pge);
	}

}

#define PGE_FONT_DECLARED 1
#endif
//! END DECLARATION