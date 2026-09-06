#include "host_wxwidgets.h"


//! START IMPLEMENTATION
namespace olc::wx
{
	PGE3Core::PGE3Core(wxWindow* parent)
	{
	}

	PGE3Core::~PGE3Core()
	{
	}

	bool PGE3Core::CreateImage(olc::Image& image, const olc::vi2d& size, const ImageConfig& cfg)
	{
		// Create CPU Image
		if (!image.CreateNoGPU(size, cfg))
			return false;

		// Create GPU Image
		auto id = pRenderer->CreateTexture(image.Size(), cfg);
		if (id == 0)
		{
			image.CreateNoGPU({ 0,0 });
			return false;
		}

		// Associate CPU object with GPU Resource
		image.SetGPUID(id);
		return true;
	}

	bool PGE3Core::CreateImageFromFile(olc::Image& image, const std::string& sFileName, const ImageConfig& cfg)
	{
		if (pImageLoader->CreateImageFromFile(image, sFileName))
		{
			// Image has loaded ok, and populated into pixel vector
			// 
			// Create GPU Image
			auto id = pRenderer->CreateTexture(image.Size(), cfg);
			if (id == 0)
			{
				image.CreateNoGPU({ 0,0 });
				return false;
			}

			// Associate CPU object with GPU Resource
			image.SetGPUID(id);
			return true;
		}

		return false;
	}

	bool PGE3Core::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes, const ImageConfig& cfg)
	{
		if (pImageLoader->CreateImageFromMemory(image, data, bytes))
		{
			// Image has loaded ok, and populated into pixel vector
			// 
			// Create GPU Image
			auto id = pRenderer->CreateTexture(image.Size(), cfg);
			if (id == 0)
			{
				image.CreateNoGPU({ 0,0 });
				return false;
			}

			// Associate CPU object with GPU Resource
			image.SetGPUID(id);
			return true;
		}

		std::cout << "Create From Memory Failed\n";
		return false;
	}

	bool PGE3Core::WriteImageToFile(const olc::Image& image, const std::string& sFileName)
	{
		olc_IgnoreUnused(image, sFileName);
		return false;
	}

	void PGE3Core::DestroyImage(olc::Image& image)
	{
		// If image has gpu resource, remove it
		if (image.GetGPUID() != 0)
		{
			pRenderer->DeleteTexture(uint32_t(image.GetGPUID()));
			image.SetGPUID(0);
		}

		// Free any cpu memory associated with image
		image.CreateNoGPU({ 0,0 });
	}

	PGE3Panel::PGE3Panel(wxWindow* parent, olc::wx::PGE3Core* core)
	{
	}

	PGE3Panel::~PGE3Panel()
	{
	}

	void PGE3Panel::Event_OnPaint(wxPaintEvent& evt)
	{
	}

	void PGE3Panel::OnPreRender()
	{
	}

	void PGE3Panel::OnRender()
	{
	}

	void PGE3Panel::OnPostRender()
	{
	}


}
//! END IMPLEMENTATION
