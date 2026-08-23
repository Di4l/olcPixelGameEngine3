#pragma once

//! START STDHEADER
#include <vector>
#include <unordered_map>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "hw_input.h"
#include "vector2d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_HW_TOUCH_DECLARED)
namespace olc
{
	// Forward declare for friendship
	class Window;
	class PGEWindow;


	namespace hw
	{
		struct TouchPoint
		{
			olc::vf2d position;
			olc::vf2d size;
			float pressure = 0;
			float orientation = 0;
			olc::vf2d tilt;
			bool bPressed = false;
			bool bReleased = false;
			bool bHeld = false;
			bool bStylus = false;
		};

		class Touch
		{
			friend class olc::Window;
			friend class olc::PGEWindow;

		public:
			Touch() = default;

		public:
			const std::vector<uint32_t>& GetTouchIDs() const;
			const bool IsTouch(const uint32_t nID) const;
			const TouchPoint& GetTouch(const uint32_t nID) const;

		protected:
			std::unordered_map<uint32_t, TouchPoint> touches_cache{};
			std::unordered_map<uint32_t, TouchPoint> touches_live{};


		private:
			void UpdateTouch(const uint32_t nID, const olc::vf2d& vPos, const bool bPress, const bool bRelease, 
				const olc::vf2d& vSize, const bool stylus = false, const float pressure = 0.0f, const float orientation = 0, const olc::vf2d& tilt = { 0,0 });
			void UpdateState();
		};
	}
}
#define PGE_HW_TOUCH_DECLARED 1
#endif
