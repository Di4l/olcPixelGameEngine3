#include "core.h"

#include "gpu_opengl33.h"
#include "host_win_winapi.h"
#include "imload_wingdi.h"

//! START IMPLEMENTATION
namespace olc
{
	bool PGEWindow::Create(const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize)
	{
		CreateImage(GetDefaultImage(), vScreenSize);
		SetWindowSize(vScreenSize * vPixelSize);
		return true;
	}

	bool PGEWindow::OnUserCreate()
	{
		return true;
	}

	bool PGEWindow::OnUserUpdate(float fElapsedTime)
	{
		return true;
	}

	bool PGEWindow::OnUserDestroy()
	{
		return true;
	}

	bool PGEWindow::olc_WindowUpdate(const float fElapsedTime)
	{
		// Input Changes
		mouse.UpdateState();


		draw.SetGPU(pRenderer);
		draw.SetTarget(GetDefaultImage());


		pRenderer->ApplyDefaultShader();



		// User Update
		if (!OnUserUpdate(fElapsedTime) || bRequestToClose)
		{
			// User has requested termination of window by returning false
			if (OnUserDestroy())
			{
				// User has confirmed window destruction by returning true
				bShouldRemove = true;
			}
			else
				bRequestToClose = false; // User vetoed closure
		}


		// Finialise any outstanding tasks
		draw.ProcessGPUTasks();

		// Take the window's completed "screen" and draw it as a textured quad to the backbuffer
		pRenderer->AssignTextureTarget(0, 0);
		pRenderer->SetViewport({ 0,0 }, GetWindowSize());
		pRenderer->ClearViewport(olc::Colour::MAGENTA, true, true);
		
		draw.WorldReset();		
		draw.ImageRect(imgPrimary, { 0.0,0.0 }, GetWindowSize());
		draw.ProcessGPUTasks();

		// Update Window's primary surface
		pRenderer->DisplayDraw();

		return true;
	}

	bool PGEWindow::CreateImage(olc::Image& image, const olc::vi2d& size, const ImageConfig& cfg)
	{
		// Create CPU Image
		if (!image.Create(size, cfg))
			return false;

		// Create GPU Image
		auto id = pRenderer->CreateTexture(image.Size(), cfg);
		if (id == 0)
		{
			image.Create({ 0,0 });
			return false;
		}

		// Associate CPU object with GPU Resource
		image.SetGPUID(id);
		return true;
	}

	bool PGEWindow::CreateImageFromFile(olc::Image& image, const std::string& sFileName, const ImageConfig& cfg)
	{
		if (pImageLoader->CreateImageFromFile(image, sFileName))
		{
			// Image has loaded ok, and populated into pixel vector
			// 
			// Create GPU Image
			auto id = pRenderer->CreateTexture(image.Size(), cfg);
			if (id == 0)
			{
				image.Create({ 0,0 });
				return false;
			}

			// Associate CPU object with GPU Resource
			image.SetGPUID(id);
			return true;
		}

		return false;
	}

	bool PGEWindow::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes, const ImageConfig& cfg)
	{
		return false;
	}

	bool PGEWindow::WriteImageToFile(const olc::Image& image, const std::string& sFileName)
	{
		return false;
	}

	void PGEWindow::DestroyImage(olc::Image& image)
	{
		// If image has gpu resource, remove it
		if (image.GetGPUID() != 0)
		{
			pRenderer->DeleteTexture(image.GetGPUID());
			image.SetGPUID(0);
		}

		// Free any cpu memory associated with image
		image.Create({ 0,0 });
	}

	void PGEWindow::LinkToRenderer(olc::gpu::Renderer* gpu)
	{
		pRenderer = gpu;
	}

	void PGEWindow::LinkToImageLoader(olc::imload::ImageLoader* imload)
	{
		pImageLoader = imload;
	}

	olc::Image& PGEWindow::GetDefaultImage()
	{
		return imgPrimary;
	}

	bool PGEWindow::olc_OnMouseMove(const olc::vi2d& vMousePos)
	{
		mouse.SetPosition(olc::vf2d(vMousePos) / olc::vf2d(GetWindowSize()) * GetDefaultImage().Size());
		return true;
	}








	PixelGameEngine::PixelGameEngine() : PGEWindow()
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
		return true;
	}

	bool PixelGameEngine::Start()
	{
		// Initialise Host Interface
		host = std::make_unique<olc::host::Host_Windows_WinAPI>();
		

		
		
		
		coreActive = true;
		coreThread = std::thread(&PixelGameEngine::EngineThread, this);

		

		// Gracefully terminate the main engine thread
		//coreActive = false;
		coreThread.join();

		return true;
	}

	bool PixelGameEngine::AddChildWindow(std::shared_ptr<olc::PGEWindow> window)
	{
		// Link this olc::Window to a host resource
		if (host)
		{
			host->AddWindowFrame(window.get(), {30,30}, config.vPixelSize * config.vScreenSize, false);
			window->LinkToHost(host.get());
			window->LinkToRenderer(gpu.get());
			window->LinkToImageLoader(imageloader.get());
			window->Create({ 64, 64 }, { 8,8 });
			deqChildWindows.push_back(window);
		}

		return true;
	}


	void PixelGameEngine::EngineThread()
	{
		using namespace std::chrono_literals;
		timeFrame2 = std::chrono::steady_clock::now();
		timeFrame1 = std::chrono::steady_clock::now();

		// Create Primary Window
		host->AddWindowFrame(this, { 30,30 }, config.vPixelSize * config.vScreenSize, false);

		// Initialise ImageLoader Interface
		imageloader = std::make_unique<olc::imload::ImageLoader_WinGDI>();
		

		// Initialise GPU Interface	- This thread is the context
		olc::gpu::RendererConfig cfgRenderer;
		cfgRenderer.VerticalSync = config.bVSync;

		gpu = std::make_unique<olc::gpu::Renderer_OGL33>();

		// Link this windows devices
		LinkToHost(host.get());
		LinkToRenderer(gpu.get());
		LinkToImageLoader(imageloader.get());

		// The GPU device can be based upon the primary window configuration. This
		// gives us completed gpu and host objects to pass to other windows as and
		// when required
		gpu->CreateDevice(host->GetHostWindowDescriptor(this), cfgRenderer);
		if (gpu->GetLastError() != olc::gpu::RendererError::None)
		{
			const auto e = gpu->GetLastError(); // For debug visibility
			std::cout << "Error: Could not create Renderer\n";
			return;
		}


		CreateImage(GetDefaultImage(), config.vScreenSize);
		

		draw.SetGPU(gpu.get());
		gpu->ApplyDefaultShader();
		draw.SetTarget(GetDefaultImage());

		if (!OnUserCreate())
		{
			// Creation process signalled abort
			return;
		}
		
		draw.ProcessGPUTasks();
		draw.SetTarget(GetDefaultImage());

		// Initialise Input Devices


		durationFrameCount = 0s;

		while (coreActive)
		{
			// This will block depending upon the system
			host->StartSystemEventLoop();

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
				SetWindowTitle(sTitle);
				frameCount = 0;
			}
						

			// Primary Window
			if (olc_ShouldRemove())
			{
				// Application is to be terminated as primary window has closed
				coreActive = false;
			}
			else
			{
				// Update Child Windows (if any)
				for (auto& winChild : deqChildWindows)
					winChild->olc_WindowUpdate(fDT);

				// Remove child windows that have requested closure
				if (!deqChildWindows.empty())
				{
					deqChildWindows.erase(std::remove_if(deqChildWindows.begin(), deqChildWindows.end(),
						[this](const std::shared_ptr<PGEWindow>& w)
						{
							if (w->olc_ShouldRemove())
							{
								host->CloseWindowFrame(w.get());
							}
							return w->olc_ShouldRemove();
						}
					), deqChildWindows.end());
				}

				// Update Primary Window
				olc_WindowUpdate(fDT);

				// Wait for vertical sync if required. 
				// Note: Child windows will never vsync as waiting for each buffer swap with vsync
				// divides up the frame rate budget across the windows.
				if (gpu->GetConfig().VerticalSync)
				{
					host->SyncWithDesktopComposite();
				}
			}

			
		}
	}
}
//! END IMPLEMENTATION