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
//! END STDHEADER GLOBAL

//! START CUSTOMHEADER GLOBAL
#include "config.h"
#include "pixel.h"
#include "vector2d.h"
#include "window.h"
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

	class PixelGameEngine : protected Window
	{
	public:
		PixelGameEngine();
		virtual ~PixelGameEngine();

	public:
		bool Construct(const olc::vi2d& vScreenSize, const olc::vi2d& vPixelSize, bool bFullScreen = false);
		bool Construct(const PGEConfig& cfg = PGEConfig{});

	public:
		bool Start();

	public:
		bool OnUserCreate() override;
		bool OnUserUpdate(float fElapsedTime) override;
		bool OnUserDestroy() override;

	private:
		void EngineThread();

	private:
		std::deque<std::unique_ptr<Window>> deqChildWindows;

		// Frame Timing & Overall Clocking
		std::chrono::steady_clock::time_point timeFrame1;
		std::chrono::steady_clock::time_point timeFrame2;
		std::chrono::duration<float> durationFrame{ 0 };

		PGEConfig config;
	};
}
#define PGE_CORE_DECLARED 1
#endif
//! END DECLARATION
