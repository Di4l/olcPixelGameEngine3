#pragma once

//! START STDHEADER GLOBAL
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <thread>
//! END STDHEADER

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER
 
//! START WINAPI_CONFIG 
#if defined(UNICODE) || defined(_UNICODE)
	#define olcT(s) L##s
#else
	#define olcT(s) s
#endif

#define _WINSOCKAPI_

#if !defined(VC_EXTRALEAN)
#define VC_EXTRALEAN
#endif

#if !defined(NOMINMAX)
#define NOMINMAX
#endif

// In Code::Blocks
#if !defined(_WIN32_WINNT)
	#ifdef HAVE_MSMF
		#define _WIN32_WINNT 0x0600 // Windows Vista
	#else
		#define _WIN32_WINNT 0x0500 // Windows 2000
	#endif
#endif

// Embrace MSVC superiority
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Dwmapi.lib")

#include <dwmapi.h>
#include <windows.h>
#undef _WINSOCKAPI_
//! END WINAPI_CONFIG

//! START DECLARATION
namespace olc
{
	namespace host
	{
		// Host for Windows OS - Single Window Only!
		class Host_Windows_WinAPI : public olc::host::Host
		{
			

		public:
			Host_Windows_WinAPI();
			virtual ~Host_Windows_WinAPI() {};


		public: // OS Window Handling
			// Make OS Create a window frame, associated with olc::Window
			bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
			// Make OS Close a window frame, associated with olc::Window
			bool CloseWindowFrame(olc::Window* pWindow) override;
			// Make OS Update a window frame title, associated with olc::Window
			bool UpdateWindowFrameTitle(olc::Window* pWindow) override;
			// Get OS-specific window descriptor(s) for given olc::Window
			std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;
			// Wait for OS desktop refresh (for smooooth vsync)
			bool SyncWithDesktopComposite() override;

		public: // Platform specific Mouse Control
			bool SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos) override;
			bool SetMouseVisible(olc::Window* pWindow, const bool bVisible) override;			
			bool SetFullScreen(olc::Window* pWindow, const bool bFullScreen) override;

		public: // OS Specific Environment Information
			olc::KeyboardLayout GetKeyboardLayout() const override;

		public:
			// Called at very start of application
			bool OnApplicationStart(olc::PixelGameEngine* pPrimary) override;
			// Called to start the host - this may mean different things on different hosts
			bool StartSystem() override;
			// Called to stop the host, and shutdown all resources
			bool StopSystem() override;
			// Called at start of system event loop
			bool OnSystemThreadStart() override;
			// Called to perform primary window update
			bool OnSystemTick() override;
			// Called at end of system event loop
			bool OnSystemThreadEnd() override;
			// Called at very end of application
			bool OnApplicationEnd() override;



		private: // Windows OS Specific Things
			std::unordered_map<size_t, HWND> mapUID2HWND;
			std::unordered_map<HWND, olc::Window*> mapHWND2PTR;
			std::wstring ConvertS2W(std::string s);
			std::atomic<bool> systemActive = false;
			HCURSOR hCursorDefault = nullptr;
			HCURSOR hCursorNow = nullptr;
			DWORD ConvertPGE2WindowStyle(const olc::Window* pWindow);

		public:
			LRESULT OnWindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

			// Map of system keycodes to olc::Keycodes
			std::unordered_map<int32_t, olc::Key> mapKeys;

		};
	}
}
//! END DECLARATION