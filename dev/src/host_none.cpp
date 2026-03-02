#include "host_none.h"

//! START IMPLEMENTATION
namespace olc::host
{
	// Make OS Create a window frame, associated with olc::Window
	bool Host_None::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
	{
		return true;
	}

	// Make OS Close a window frame, associated with olc::Window
	bool Host_None::CloseWindowFrame(olc::Window* pWindow)
	{
		return true;
	}

	// Make OS Update a window frame title, associated with olc::Window
	bool Host_None::UpdateWindowFrameTitle(olc::Window* pWindow)
	{
		return true;
	}

	// Get OS-specific window descriptor(s) for given olc::Window
	std::vector<void*> Host_None::GetHostWindowDescriptor(olc::Window* pWindow)
	{
		return {};
	}

	// Wait for OS desktop refresh (for smooooth vsync)
	bool Host_None::SyncWithDesktopComposite()
	{
		return true;
	}

	// Force the mouse position in pixels relative to window
	bool Host_None::SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos)
	{
		return true;
	}
	
	// Show or hide mouse cursor for given window
	bool Host_None::SetMouseVisible(olc::Window* pWindow, const bool bVisible)
	{
		return true;
	}

	olc::KeyboardLayout Host_None::GetKeyboardLayout() const
	{
		return OLC_DEFAULT_KEYBOARD_LAYOUT;
	}

	// Called at very start of application
	bool Host_None::OnApplicationStart(olc::PixelGameEngine* pPrimary)
	{
		pPrimaryPGE = pPrimary;
		return true;
	}

	// Called to start the host - this may mean different things on different hosts
	// It MUST block until system is requested to exit
	bool Host_None::StartSystem()
	{
		pPrimaryPGE->OnPreContextStart();

		systemActive = true;

		if(!OnSystemThreadStart())
			return pPrimaryPGE->OnPostContextEnd();
		
		while(systemActive)
		{
			if(!OnSystemTick())
			{
				StopSystem();
			}
		}

		OnSystemThreadEnd();

		return pPrimaryPGE->OnPostContextEnd();
	}

	// Called to stop the host, and shutdown all resources
	bool Host_None::StopSystem()
	{
		systemActive = false;
		return true;
	}

	// Called at start of system event loop
	bool Host_None::OnSystemThreadStart()
	{
		return pPrimaryPGE->OnContextStart();
	}

	// Called to perform primary window update
	bool Host_None::OnSystemTick()
	{
		return pPrimaryPGE->OnContextTick();
	}
	
	// Called at end of system event loop
	bool Host_None::OnSystemThreadEnd()
	{
		return pPrimaryPGE->OnContextEnd();
	}
	
	// Called at very end of application
	bool Host_None::OnApplicationEnd()
	{
		return true;
	}
}
//! END IMPLEMENTATION