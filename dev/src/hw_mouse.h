#pragma once

//! START STDHEADER
#include <array>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "hw_input.h"
#include "vector2d.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_HW_MOUSE_DECLARED)
namespace olc
{
	// Forward declare for friendship
	class Window;		


	namespace hw
	{
		class Mouse
		{
			friend class olc::Window;

		public:
			Mouse() = default;

		public:
			const Button& GetButton(const int nButton) const;
			const olc::vf2d& GetPosition() const;

		protected:
			std::array<Button, OLC_MOUSE_BUTTONS> buttons{};
			std::array<bool, OLC_MOUSE_BUTTONS> buttons_new{};
			std::array<bool, OLC_MOUSE_BUTTONS> buttons_old{};
			olc::vf2d position;
			olc::vf2d position_in;

		private:
			void SetPosition(const olc::vf2d& pos);
			void SetButton(const int nButton, bool state);
			void UpdateState();
		};
	}
}
#define PGE_HW_MOUSE_DECLARED 1
#endif
