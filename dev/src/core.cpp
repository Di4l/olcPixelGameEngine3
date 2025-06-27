#include "core.h"

//! START IMPLEMENTATION
namespace olc
{
	PixelGameEngine::PixelGameEngine()
	{
	}

	PixelGameEngine::~PixelGameEngine()
	{
	}

	bool PixelGameEngine::Construct(const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize, bool bFullScreen)
	{
		config.vScreenSize = vScreenSize;
		config.vPixelSize = vPixelSize;
		config.bFullScreen = bFullScreen;

		return false;
	}

	bool PixelGameEngine::Construct(const PGEConfig& cfg)
	{		
		config = cfg;

		return false;
	}

	bool PixelGameEngine::Start()
	{
		return false;
	}

	bool PixelGameEngine::OnUserCreate()
	{
		return false;
	}

	bool PixelGameEngine::OnUserUpdate(float fElapsedTime)
	{
		return false;
	}

	bool PixelGameEngine::OnUserDestroy()
	{
		return false;
	}

	void PixelGameEngine::EngineThread()
	{
		while (true)
		{
			// Frame Delta Timing - "ElapsedTime" since last core update
			// ~~~~~~~~~~~~~~~~~~
			// All timing is synchronous to the primary window, i.e child
			// windows do not maintain their own frame timing. This is a
			// deliberate decision as PGE will maintain sync between windows.
			// Why? Multiple windows are not really the point of PGE and
			// indeed could just be addional unnecessary complexity. However,
			// by moving to a Window abstraction we kinda get it for free.
			// This freedom comes at the expense of complexity. If we allowed
			// windows to be wholly isolated from the core loop, then the
			// user is expected to maintain thread safety, context sharing
			// and resource management. That is not olc::PGE.

			timeFrame1 = std::chrono::steady_clock::now();
			durationFrame = timeFrame2 - timeFrame1;
			timeFrame1 = timeFrame2;

			// Our time per frame coefficient
			float fDT = durationFrame.count();

			// Primary Window
			olc_WindowUpdate(fDT);

			// Child Windows
			for (auto& winChild : deqChildWindows)
				winChild->olc_WindowUpdate(fDT);

			// Remove child windows that have requested closure
			deqChildWindows.erase(std::remove_if(deqChildWindows.begin(), deqChildWindows.end(),
				[](const std::unique_ptr<Window>& w)
				{
					return w->olc_ShouldRemove();
				}
			));
		}
	}
}
//! END IMPLEMENTATION