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
#include "gpu_iface.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_WINDOW_DECLARED)

#if OLC_HOST == OLC_HOST_WINDOWS
	#define FRIENDLY_HOST Host_Windows_WinAPI
#endif

namespace olc
{
	namespace host
	{
		class FRIENDLY_HOST;
		class Host;
	}

	class Window : public std::enable_shared_from_this<olc::Window>
	{
		friend class olc::host::FRIENDLY_HOST;

	public:
		Window();
		virtual ~Window();

		void ConnecToHost(olc::host::Host* host);

	public:
		virtual bool OnUserCreate();
		virtual bool OnUserUpdate(float fElapsedTime);
		virtual bool OnUserDestroy();

	private:
		// Set Mouse Device State
		bool olc_OnMouseButton(const uint8_t nButton, const bool bPressed);
		bool olc_OnMouseMove(const olc::vi2d& vMousePos);
		bool olc_OnMouseWheel(const int32_t nScroll);
		bool olc_OnMouseFocus(const bool bHasFocus);
		
		// Set Window State
		bool olc_OnWindowPosition(const olc::vi2d& vWindowPos);
		bool olc_OnWindowSize(const olc::vi2d& vWindowSize);

		// Set Keyboard State



	public:
		bool olc_ShouldRemove() const;
		bool olc_WindowUpdate(const float fElapsedTime);

	public:
		const size_t GetUID() const;

		const olc::vi2d& GetSize() const;
		bool SetSize(const olc::vi2d& vSize);

		const olc::vi2d& GetPosition() const;
		bool SetPosition(const olc::vi2d& vPosition);

		const std::string& GetTitle() const;
		bool SetTitle(const std::string& sTitle);


	protected:
		virtual bool olc_PrimaryWindowInit();

	protected:
		bool bShouldRemove = false;

	protected:
		size_t nUniqueID = -1;
		olc::vi2d vWindowPos;
		olc::vi2d vWindowSize;
		std::string sFrameTitle;
	
	private:
		olc::vi2d volatile_vMousePos;
		
		olc::host::Host* pHost = nullptr;
		//olc::gpu::Renderer* pRenderer = nullptr;
	};
}
#define PGE_WINDOW_DECLARED 1
#endif
//! END DECLARATION