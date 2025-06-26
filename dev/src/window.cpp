#include "window.h"

//! START IMPLEMENTATION
namespace olc
{
	Window::Window()
	{
	}

	Window::~Window()
	{
	}

	bool Window::OnUserCreate()
	{
		return false;
	}

	bool Window::OnUserUpdate(float fElapsedTime)
	{
		return false;
	}

	bool Window::OnUserDestroy()
	{
		return false;
	}

	bool Window::olc_ShouldRemove() const
	{
		return bShouldRemove;
	}

	bool Window::olc_WindowUpdate(const float fElapsedTime)
	{
		// Environmental changes

		// User Update
		if (!OnUserUpdate(fElapsedTime))
		{
			// User has requested termination of window by returning false
			if (OnUserDestroy())
			{
				// User has confirmed window destruction by returning true
				bShouldRemove = true;
			}
		}

		return false;
	}
};
//! END IMPLEMENTATION