#pragma once

//! START STDHEADER GLOBAL
#include <cstdint>
#include <vector>
#include <optional>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "pixel.h"
#include "vector2d.h"
#include "window.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_HOST_IFACE_DECLARED)
namespace olc
{
	namespace pgeguts
	{
		inline static size_t uuid = 0;

		inline constexpr size_t CreateUID()
		{
			return uuid++;
		}	
	}

	namespace host
	{

		

		struct HostConfig
		{
			
		};

		enum class HostError
		{
			None,
			
		};

		class Host
		{
		public:
			Host() = default;
			virtual ~Host() {};

		public:
			// Check/Get last error
			HostError GetLastError() const { return lastError; }

		public: 
			virtual bool StartSystemEventLoop() = 0;
			virtual bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) = 0;
			
			virtual bool UpdateWindowFrameTitle(olc::Window* pWindow) = 0;

			virtual std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) = 0;
			
			
			virtual bool ConnectHostResourceToRenderer() = 0;

			// Wait for entire host desktop refresh (for smooooth vsync)
			virtual bool SyncWithDesktopComposite() = 0;

		protected:
			HostError lastError = HostError::None;
		};
	}
}
#define PGE_HOST_IFACE_DECLARED 1
#endif
//! END DECLARATION