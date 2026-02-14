#pragma once

//! START STDHEADER GLOBAL
//! END STDHEADER

//! START CUSTOMHEADER
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_EXTENSION_DECLARED)
namespace olc
{
	class PixelGameEngine;
	class PGEWindow;

	namespace hw
	{
		class Mouse;
		class Keyboard;
	}

	// System level extension
	class PGESystemExtension
	{
		friend class olc::PixelGameEngine;
		friend class olc::PGEWindow;
		friend class olc::hw::Mouse;
		friend class olc::hw::Keyboard;

	private:
		// Called when extension is installed, usually in PGE Constructor
		// Return true to continue application
		virtual bool OnInstall([[maybe_unused]] olc::PixelGameEngine* pge) { return true; }
		// Called after PGE is established, but before OnUserCreate()
		// Return true to continue application
		virtual bool OnBeforeUserCreate([[maybe_unused]] olc::PixelGameEngine* pge) { return true; }
		// Called after OnUserCreate(), but before the first call to OnUserUpdate()
		// Return true to continue application
		virtual bool OnAfterUserCreate([[maybe_unused]] olc::PixelGameEngine* pge) { return true; }
		// Called at the start of each frame, before OnUserUpdate()
		// Return true if you wish to block OnUserUpdate() from being called this frame
		virtual bool OnBeforeSystemUpdate([[maybe_unused]] olc::PixelGameEngine* pge, [[maybe_unused]] float fElapsedTime) { return false; }
		// Called at the end of each frame, after OnUserUpdate()
		// Return true to continue application
		virtual bool OnAfterSystemUpdate([[maybe_unused]] olc::PixelGameEngine* pge, [[maybe_unused]] float fElapsedTime) { return true; }
	};

	// Window level extension
	class PGEWindowExtension
	{
		friend class olc::PGEWindow;
		friend class olc::hw::Mouse;
		friend class olc::hw::Keyboard;

	private:
		// Called when extension is installed, usually in PGE Constructor
		// Return true to continue application
		virtual bool OnInstall([[maybe_unused]] olc::PGEWindow* pge) { return true; }
		// Called after PGE is established, but before OnUserCreate()
		// Return true to continue application
		virtual bool OnBeforeUserCreate([[maybe_unused]] olc::PGEWindow* pge) { return true; }
		// Called after OnUserCreate(), but before the first call to OnUserUpdate()
		// Return true to continue application
		virtual bool OnAfterUserCreate([[maybe_unused]] olc::PGEWindow* pge) { return true; }
		// Called at the start of each frame, before OnUserUpdate()
		// Return true if you wish to block OnUserUpdate() from being called this frame
		virtual bool OnBeforeUserUpdate([[maybe_unused]] olc::PGEWindow* pge, [[maybe_unused]] float &fElapsedTime) { return false; }
		// Called at the end of each frame, after OnUserUpdate()
		// Return true to indicate you have modified something
		virtual bool OnAfterUserUpdate([[maybe_unused]] olc::PGEWindow* pge, [[maybe_unused]] float fElapsedTime) { return false; }
	};
}
#define PGE_EXTENSION_DECLARED 1
#endif
//! END DECLARATION