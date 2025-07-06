#include "core.h"

#include "gpu_opengl33.h"
#include "host_win_winapi.h"

//! START IMPLEMENTATION
namespace olc
{
	PixelGameEngine::PixelGameEngine() : Window()
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

		return true;
	}

	bool PixelGameEngine::Construct(const PGEConfig& cfg)
	{		
		config = cfg;

		return false;
	}

	bool PixelGameEngine::Start()
	{
		// Initialise Host Interface
		host = std::make_unique<olc::host::Host_Windows_WinAPI>();
		

		// Link this olc::Window to a host resource
		if (host)
			host->AddWindowFrame(this, { 30,30 }, { 250, 240 }, false);
		
		coreActive = true;
		coreThread = std::thread(&PixelGameEngine::EngineThread, this);

		// This will block depending upon the system
		host->StartSystemEventLoop();

		// Gracefully terminate the main engine thread
		coreActive = false;
		coreThread.join();

		return true;
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

	bool PixelGameEngine::olc_PrimaryWindowInit()
	{
		// Called by window to initialise core systems. Sub windows should ignore this
		return false;
	}

	void PixelGameEngine::EngineThread()
	{
		using namespace std::chrono_literals;
		timeFrame2 = std::chrono::steady_clock::now();
		timeFrame1 = std::chrono::steady_clock::now();
		

		// Initialise GPU Interface	- This thread is the context
		olc::gpu::RendererConfig cfgRenderer;
		gpu = std::make_unique<olc::gpu::Renderer_OGL33>();
		gpu->CreateDevice({ nullptr }, cfgRenderer);


		// Initialise Input Devices


		durationFrameCount = 0s;

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
			durationFrame = timeFrame1 - timeFrame2;
			timeFrame2 = timeFrame1;

			// Our time per frame coefficient
			float fDT = durationFrame.count();

			frameCount++;
			durationFrameCount += durationFrame;
			
			if (durationFrameCount >= 1s)
			{
				durationFrameCount -= 1s;
				std::string sTitle = "OneLoneCoder.com - Pixel Game Engine 3 - Test - FPS: " + std::to_string(frameCount);
				SetTitle(sTitle);
				frameCount = 0;
			}

			// Primary Window
			olc_WindowUpdate(fDT);

			// Child Windows
			for (auto& winChild : deqChildWindows)
				winChild->olc_WindowUpdate(fDT);

			// Remove child windows that have requested closure
			if (!deqChildWindows.empty())
			{
				deqChildWindows.erase(std::remove_if(deqChildWindows.begin(), deqChildWindows.end(),
					[](const std::shared_ptr<Window>& w)
					{
						return w->olc_ShouldRemove();
					}
				));
			}
		}
	}
}
//! END IMPLEMENTATION