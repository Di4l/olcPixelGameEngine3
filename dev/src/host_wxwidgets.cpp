#include "host_wxwidgets.h"


//! START IMPLEMENTATION
namespace olc::wx
{
	PGE3Core::PGE3Core(wxWindow* parent) : wxGLCanvas(parent, -1, nullptr)
	{
		// Create a wxWidgets "glContext" targeting OpenGL 3.3
		wxGLContextAttrs ctxAttrs;
		ctxAttrs.CoreProfile().OGLVersion(3, 3).EndList();
		m_glContext = new wxGLContext(this, NULL, &ctxAttrs);
		SetCurrent(*m_glContext);


		// Create all the vital parts of PGE
		pImageLoader = std::make_unique<olc::imload::OLC_IMAGELOADER_CLASS>();

		// Create the renderer
		olc::gpu::RendererConfig cfgRenderer;
		pRenderer = std::make_unique<olc::gpu::OLC_GPU_CLASS>();
		pRenderer->CreateDevice({ this }, cfgRenderer);

		// Load the font
		olc::pgeguts::CreateClassicFont(this);

		// wxWidgets should hide this pseudo-canvas. It was only
		// needed to get the context
		this->Hide();
	}

	PGE3Core::~PGE3Core()
	{
		delete m_glContext;
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

	wxGLContext* PGE3Core::get() const
	{
		return m_glContext;
	}

	olc::gpu::Renderer* PGE3Core::GetRenderer()
	{
		return pRenderer.get();
	}

	olc::imload::ImageLoader* PGE3Core::GetImageLoader()
	{
		return pImageLoader.get();
	}



	olc::wx::PGE3Core* PGE3Panel::m_pCore = nullptr;



	PGE3Panel::PGE3Panel(wxWindow* parent) : wxGLCanvas(parent, -1, nullptr)
	{
		if (m_pCore == nullptr)
		{
			// Create a static PGE3Core
			m_pCore = new PGE3Core(this);
		}

		// Set the context via wxWidgets
		SetCurrent(*m_pCore->get());

		// Associate this instance of draw with the renderer
		draw.SetGPU(m_pCore->GetRenderer());

		// Create the default draw target
		bFixedSize = false;
		m_pCore->CreateImage(imgPrimary, { 256, 240 });
		
		ResetDrawState();

		Connect(wxEVT_PAINT, wxPaintEventHandler(olc::wx::PGE3Panel::Event_OnPaint));
		Connect(wxEVT_SIZE, wxSizeEventHandler(olc::wx::PGE3Panel::Event_OnResize));
		Connect(wxEVT_MOTION, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseMove));
		Connect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseMiddleDown));
		Connect(wxEVT_MIDDLE_UP, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseMiddleUp));
		Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseWheel));
		Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseLeftDown));
		Connect(wxEVT_LEFT_UP, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseLeftUp));
		Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseRightDown));
		Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseRightUp));
	}

	PGE3Panel::PGE3Panel(wxWindow* parent, const olc::vi2d& vFixedSize) : wxGLCanvas(parent, -1, nullptr)
	{
		if (m_pCore == nullptr)
		{
			// Create a static PGE3Core
			m_pCore = new PGE3Core(this);
		}

		// Set the context via wxWidgets
		SetCurrent(*m_pCore->get());

		// Associate this instance of draw with the renderer
		draw.SetGPU(m_pCore->GetRenderer());

		// Create the default draw target
		vFixedSizeImage = vFixedSize;
		bFixedSize = true;
		m_pCore->CreateImage(imgPrimary, vFixedSize);

		ResetDrawState();

		Connect(wxEVT_PAINT, wxPaintEventHandler(olc::wx::PGE3Panel::Event_OnPaint));		
		Connect(wxEVT_MOTION, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseMove));
		Connect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseMiddleDown));
		Connect(wxEVT_MIDDLE_UP, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseMiddleUp));
		Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseWheel));
		Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseLeftDown));
		Connect(wxEVT_LEFT_UP, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseLeftUp));
		Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseRightDown));
		Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseRightUp));
	}

	PGE3Panel::~PGE3Panel()
	{
		Disconnect(wxEVT_PAINT, wxPaintEventHandler(olc::wx::PGE3Panel::Event_OnPaint));
		Disconnect(wxEVT_SIZE, wxSizeEventHandler(olc::wx::PGE3Panel::Event_OnResize));
		Disconnect(wxEVT_MOTION, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseMove));
		Disconnect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseMiddleDown));
		Disconnect(wxEVT_MIDDLE_UP, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseMiddleUp));
		Disconnect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseWheel));
		Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseLeftDown));
		Disconnect(wxEVT_LEFT_UP, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseLeftUp));
		Disconnect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseRightDown));
		Disconnect(wxEVT_RIGHT_UP, wxMouseEventHandler(olc::wx::PGE3Panel::Event_OnMouseRightUp));
	}

	void PGE3Panel::ResetDrawState()
	{
		// Set the context via wxWidgets
		SetCurrent(*m_pCore->get());
		m_pCore->GetRenderer()->ApplyDefaultShader();
		draw.SetTarget(imgPrimary);
		draw.WorldReset();
		draw.SetBlendMode(olc::BlendMode::Alpha);
		draw.SetCullMode(olc::CullMode::None);
	}

	

	void PGE3Panel::Event_OnPaint(wxPaintEvent& evt)
	{
		// Configure WX to render this window
		wxPaintDC(this);

		// Perform actual render
		ResetDrawState();
		OnRender();
		draw.ProcessGPUTasks();

		// Present image to screen in location of panel
		m_pCore->GetRenderer()->AssignTextureTarget(0, 0);

		olc::vf2d vViewPos = { 0,0 };
		olc::vf2d vViewSize = { float(GetSize().x), float(GetSize().y) };

		// Present final composite
		m_pCore->GetRenderer()->SetViewport(vViewPos, vViewSize);
		m_pCore->GetRenderer()->ClearViewport(olc::Colour::RED, true, true);
		draw.SetBlendMode(olc::BlendMode::Alpha);
		draw.SetCullMode(olc::CullMode::None);
		draw.ImageRect(imgPrimary.flipV(), { 0.0,0.0 }, vViewSize);
		draw.ProcessGPUTasks();

		// Update Window's primary surface - via wxWidgets
		SwapBuffers();
	}

	void PGE3Panel::Event_OnResize(wxSizeEvent& evt)
	{
		if (!bFixedSize)
		{
			// Resize Target Image
			SetCurrent(*m_pCore->get());
			m_pCore->DestroyImage(imgPrimary);
			m_pCore->CreateImage(imgPrimary, { evt.GetSize().x, evt.GetSize().y });
		}

		Refresh(true);
		evt.Skip(true);
	}

	olc::vf2d PGE3Panel::ScaleMouse(wxMouseEvent& evt)
	{
		olc::vf2d vScreenPos = olc::vf2d(float(evt.GetX()), float(evt.GetY()));

		if (bFixedSize)
			vScreenPos = vScreenPos / olc::vf2d(GetSize().x, GetSize().y) * vFixedSizeImage;

		return vScreenPos;
	}

	void PGE3Panel::Event_OnMouseLeftUp(wxMouseEvent& evt)
	{
		OnMouseLeftUp(draw.ScreenToWorld(ScaleMouse(evt)), evt.ShiftDown(), evt.ControlDown());
		evt.Skip();
	}

	void PGE3Panel::Event_OnMouseLeftDown(wxMouseEvent& evt)
	{
		OnMouseLeftDown(draw.ScreenToWorld(ScaleMouse(evt)), evt.ShiftDown(), evt.ControlDown());
		evt.Skip();
	}

	void PGE3Panel::Event_OnMouseRightUp(wxMouseEvent& evt)
	{
		OnMouseRightUp(draw.ScreenToWorld(ScaleMouse(evt)), evt.ShiftDown(), evt.ControlDown());
		evt.Skip();
	}

	void PGE3Panel::Event_OnMouseRightDown(wxMouseEvent& evt)
	{
		OnMouseRightDown(draw.ScreenToWorld(ScaleMouse(evt)), evt.ShiftDown(), evt.ControlDown());
		evt.Skip();
	}

	void PGE3Panel::Event_OnMouseMiddleUp(wxMouseEvent& evt)
	{
		OnMouseMiddleUp(draw.ScreenToWorld(ScaleMouse(evt)), evt.ShiftDown(), evt.ControlDown());
		evt.Skip();
	}

	void PGE3Panel::Event_OnMouseMiddleDown(wxMouseEvent& evt)
	{
		OnMouseMiddleDown(draw.ScreenToWorld(ScaleMouse(evt)), evt.ShiftDown(), evt.ControlDown());
		evt.Skip();
	}

	void PGE3Panel::Event_OnMouseMove(wxMouseEvent& evt)
	{
		OnMouseMove(draw.ScreenToWorld(ScaleMouse(evt)), evt.ShiftDown(), evt.ControlDown());
		evt.Skip();
	}

	void PGE3Panel::Event_OnMouseWheel(wxMouseEvent& evt)
	{
		evt.Skip();
	}

}
//! END IMPLEMENTATION
