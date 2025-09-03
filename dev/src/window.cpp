#include "window.h"

#include "host_iface.h"

//! START IMPLEMENTATION
namespace olc
{
	Window::Window()
	{
		nUniqueID = pgeguts::CreateUID();
	}

	Window::~Window()
	{
	}

	void Window::ConnecToHost(olc::host::Host* host)
	{
		pHost = host;
		sFrameTitle = "OneLoneCoder.com - Pixel Game Engine 3";
		pHost->UpdateWindowFrameTitle(this);
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

	bool Window::olc_OnMouseButton(const uint8_t nButton, const bool bPressed)
	{
		return false;
	}

	bool Window::olc_OnMouseMove(const olc::vi2d& vMousePos)
	{
		volatile_vMousePos = vMousePos;
		return true;
	}

	bool Window::olc_OnMouseWheel(const int32_t nScroll)
	{
		return false;
	}

	bool Window::olc_OnMouseFocus(const bool bHasFocus)
	{
		return false;
	}

	bool Window::olc_OnWindowPosition(const olc::vi2d& vWindowPos)
	{
		return false;
	}

	bool Window::olc_OnWindowSize(const olc::vi2d& vWindowSize)
	{
		return false;
	}

	bool Window::olc_ShouldRemove() const
	{
		return bShouldRemove;
	}

	bool Window::olc_WindowUpdate(const float fElapsedTime, olc::gpu::Renderer* const gpu)
	{
		// Environmental changes

		draw.ClearTransform();
		draw.SetGPU(gpu);
		draw.SetTarget(imgPrimary);

		gpu->SetViewport({ 0,0 }, GetSize());
		gpu->ClearViewport(olc::Colour::TANGERINE, false, false);

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

		
		gpu->DisplayDraw();

		return true;
	}

	size_t Window::GetUID() const
	{
		return nUniqueID;
	}

	const olc::vi2d& Window::GetSize() const
	{
		return vWindowSize;
	}

	const olc::vi2d& Window::GetPosition() const
	{
		return vWindowPos;
	}

	bool Window::SetSize(const olc::vi2d& vSize)
	{
		// Request host to change geometry of window object

		vWindowSize = vSize;
		return true;
	}

	bool Window::SetPosition(const olc::vi2d& vPosition)
	{
		// Request host to change geometry of window object

		vWindowPos = vPosition;
		return true;
	}

	const std::string& Window::GetTitle() const
	{
		return sFrameTitle;
	}

	bool Window::SetTitle(const std::string& sTitle)
	{
		sFrameTitle = sTitle;
		pHost->UpdateWindowFrameTitle(this);
		return false;
	}

	bool Window::olc_PrimaryWindowInit()
	{
		return false;
	}
};
//! END IMPLEMENTATION