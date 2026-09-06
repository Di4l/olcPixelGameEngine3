#pragma once
#include "core.h"

//! START STDHEADER GLOBAL
#include <cstdint>
#include <vector>
#include <unordered_map>
//! END STDHEADER

//! START CUSTOMHEADER
#include "pixel.h"
#include "vector2d.h"
#include "vector4d.h"
#include "matrix3d.h"
#include "matrix4d.h"
#include "transform2d.h"
#include "image.h"
#include "imload_iface.h"
#include "gpu_iface.h"
//! END CUSTOMHEADER

//! START WXWIDGETS_CONFIG
#include <wx/wx.h>
#include <wx/glcanvas.h>
//! END WXWIDGETS_CONFIG

//! START DECLARATION
// Not really an olc::Host, but some stand alone components that wxWidgets
// can use and get a PGE3 like rendering and user experience
namespace olc::wx
{
	class PGE3Core : public wxGLCanvas
	{
	public:
		PGE3Core(wxWindow* parent);
		virtual ~PGE3Core();

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
		wxGLContext* get() const;
		olc::gpu::Renderer* GetRenderer();
		olc::imload::ImageLoader* GetImageLoader();

	private: // wxWidgets Specific
		wxGLContext* m_glContext = nullptr;

	private: // Global PGE3 Components (excluding host)				
		std::unique_ptr<olc::gpu::Renderer> pRenderer;
		std::unique_ptr<olc::imload::ImageLoader> pImageLoader;
	};

	class PGE3Panel : public wxGLCanvas
	{
	public:
		PGE3Panel(wxWindow* parent);
		PGE3Panel(wxWindow* parent, const olc::vi2d& vFixedSize);
		virtual ~PGE3Panel();

		void ResetDrawState();

	protected: // Local PGE3 Components
		olc::Draw draw;

	private:
		// Same core is loaded across all panels in application
		static olc::wx::PGE3Core* m_pCore;
		olc::Image imgPrimary;

		bool bFixedSize = false;
		olc::vi2d vFixedSizeImage;


	private: // wxWidgets Overrides
		void Event_OnPaint(wxPaintEvent& evt);
		void Event_OnResize(wxSizeEvent& evt);


	protected: // User Overrides
		virtual void OnRender(); // Called via wxWidgets   ->Refresh()

	public:
		virtual void OnCreate();
		virtual void OnUpdate(const float fElapsedTime);
	};
}
//! END DECLARATION
