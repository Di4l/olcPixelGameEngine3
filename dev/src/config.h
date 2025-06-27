#pragma once

//! START CONFIGURATION
// Version number (accessible as value for incremental comparisons)
#define PGE_VER 300



// Pixel Format in memory 													  
#define PGE_PIXEL_LAYOUT_RGBA 1
#define PGE_PIXEL_LAYOUT_ABGR 2

#if !defined(PGE_PIXEL_LAYOUT)
	#define PGE_PIXEL_LAYOUT PGE_PIXEL_LAYOUT_ABGR
#endif



// Use UK spellings where appropriate
#define PGE_SPELL_CORRECTLY 1
#define PGE_SPELL_INCORRECTLY 2

#if !defined(PGE_SPELLING)
	#define PGE_SPELLING PGE_SPELL_CORRECTLY
#endif
//! END CONFIGURATION