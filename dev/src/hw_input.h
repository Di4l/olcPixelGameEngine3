#pragma once

//! START STDHEADER

//! END STDHEADER

//! START CUSTOMHEADER

//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_HARDWAREINPUT_DECLARED)
namespace olc
{
	namespace hw
	{
		struct Button
		{
			bool bPressed = false;
			bool bReleased = false;
			bool bHeld = false;
		};
	}
}
#define PGE_HARDWAREINPUT_DECLARED 1
#endif
