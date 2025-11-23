#pragma once

//! START STDHEADER GLOBAL
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <deque>
#include <chrono>
#include <thread>
//! END STDHEADER GLOBAL

//! START CUSTOMHEADER GLOBAL
#include "config.h"
#include "pixel.h"
#include "vector2d.h"
#include "window.h"
#include "gpu_iface.h"
#include "host_iface.h"
#include "imload_iface.h"
//! END CUSTOMHEADER GLOBAL

//! START DECLARATION
#if !defined(PGE_CORE_DECLARED)
namespace olc
{
	// A grouping of all settable PGE properties
	struct PGEConfig
	{
		// Size of "screen" in PGE pixels
		olc::vi2d vScreenSize = { 256, 240 };
		// Size of a PGE pixel
		olc::vi2d vPixelSize = { 4, 4 };
		// Top left location of shown main window
		olc::vi2d vWindowOffset = { 30,30 };
		// Start in full-screen mode
		bool bFullScreen = false;
		// Allow full screen as an option with ALT-ENTER
		bool bFullScreenable = true;
		// Allow teh window to be resized by user
		bool bResizeable = true;
		// Synchronise rendering with monitor
		bool bVSync = false;
		// Behave like a host window, resizing the screen in response to window resize
		bool bRealWindow = false;
		// Ensure aspect ratio of "screen" is mainatined regardless of window size
		bool bRetainAspectRatio = true;
		// Force "screen" pixels to be integer in size
		bool bForceIntegerPixelSize = false;
		// Allow main PGE window to spawn child windows
		bool bAllowChildWindows = true;
	};

	class PGEWindow : public Window
	{
	public:
		bool Create(const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize);
	
	public:
		// Return true if window is to continue
		virtual bool OnUserCreate();
		// Return true if window is to continue
		virtual bool OnUserUpdate(float fElapsedTime);
		// Return true if window is to close
		virtual bool OnUserDestroy();
		
	
	public:	// olc::Image Handling
		// Create an image resource
		bool CreateImage(olc::Image& image, const olc::vi2d& size, const ImageConfig& cfg = olc::ImageConfig());
		// Create an image resource based on an image file asset on disk
		bool CreateImageFromFile(olc::Image& image, const std::string& sFileName, const ImageConfig& cfg = olc::ImageConfig());
		// Create an image resource based on an image file asset in memory
		bool CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes, const ImageConfig& cfg = olc::ImageConfig());
		// Store an image as a file asset on disk
		bool WriteImageToFile(const olc::Image& image, const std::string& sFileName);
		// Store an image as a file asset in memory
		//bool WriteImageToMemory(const olc::Image& image, std::vector<uint8_t> bytes, const std::string& sFileName);
		// Destroy an image
		void DestroyImage(olc::Image& image);

	public:
		void LinkToRenderer(olc::gpu::Renderer* gpu);
		void LinkToImageLoader(olc::imload::ImageLoader* imload);


		olc::Image& GetDefaultImage();
		

	protected:
		bool olc_OnMouseMove(const olc::vi2d& vMousePos) override;

	public:
		virtual bool olc_WindowUpdate(const float fElapsedTime);

	protected:
		olc::Draw2D draw;
		
		
	private:
		olc::Image imgPrimary;
		olc::gpu::Renderer* pRenderer = nullptr;
		olc::imload::ImageLoader* pImageLoader = nullptr;
	};

	class PixelGameEngine : protected PGEWindow
	{
	public:
		PixelGameEngine();
		virtual ~PixelGameEngine();

	public:
		bool Construct(const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize, bool bFullScreen = false);
		bool Construct(const PGEConfig& cfg = PGEConfig{});

	public:
		bool Start();

	public: // Child Windows
		bool AddChildWindow(std::shared_ptr<olc::PGEWindow> window);

	private:
		void EngineThread();

	private:
		std::deque<std::shared_ptr<PGEWindow>> deqChildWindows;

		// Frame Timing & Overall Clocking
		std::chrono::steady_clock::time_point timeFrame1;
		std::chrono::steady_clock::time_point timeFrame2;
		std::chrono::duration<float> durationFrame{ 0 };
		std::chrono::duration<float> durationFrameCount{ 0 };
		size_t frameCount = 0;

		PGEConfig config;

		std::thread coreThread;
		std::atomic<bool> coreActive;

		std::unique_ptr<olc::gpu::Renderer> gpu;
		std::unique_ptr<olc::host::Host> host;
		std::unique_ptr<olc::imload::ImageLoader> imageloader;
	};
}
#define PGE_CORE_DECLARED 1
#endif
//! END DECLARATION
