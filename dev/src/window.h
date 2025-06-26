#pragma once

//! START STDHEADER	GLOBAL
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <memory>
//! END STDHEADER

//! START CUSTOMHEADER GLOBAL
#include "config.h"
#include "pixel.h"
#include "vector2d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_WINDOW_DECLARED)
namespace olc
{
	class Window
	{
	public:
		Window();
		virtual ~Window();

	public:
		virtual bool OnUserCreate();
		virtual bool OnUserUpdate(float fElapsedTime);
		virtual bool OnUserDestroy();

	public:
		bool olc_ShouldRemove() const;
		bool olc_WindowUpdate(const float fElapsedTime);

	protected:
		bool bShouldRemove = false;
	};
}
#define PGE_WINDOW_DECLARED 1
#endif
//! END DECLARATION