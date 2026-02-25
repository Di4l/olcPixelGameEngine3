#pragma once
#include "core.h"

//! START STDHEADER GLOBAL
#include <cstdint>
#include <vector>
#include <optional>
//! END STDHEADER

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER

//! START DECLARATION
namespace olc::host
{
	class Host_None : public olc::host::Host
	{
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
		// Force the mouse position in pixels relative to window
		bool SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos) override;
		// Show or hide mouse cursor for given window
		bool SetMouseVisible(olc::Window* pWindow, const bool bVisible) override;

	public: // OS Specific Environment Information
		virtual olc::KeyboardLayout GetKeyboardLayout() const override;

	public: // Platform Specific OS<->PGE Linkage
		// Called at very start of application
		bool OnApplicationStart(olc::PixelGameEngine* pPrimary) override;
		// Called to start the host - this may mean different things on different hosts
		// It MUST block until system is requested to exit
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
	private:
		bool systemActive = false;
	};
}
//! END DECLARATION