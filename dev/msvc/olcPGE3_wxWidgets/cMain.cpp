#include "cMain.h"

// === BallPanel ===

BallPanel::BallPanel(wxWindow* pParent, olc::wx::PGE3Core* core) : olc::wx::PGE3Panel(pParent, core)
{
	
}


// === MouseFollowPanel ===
//
//MouseFollowPanel::MouseFollowPanel(wxWindow* pParent) : olc::wx::wxOLCPGE3Panel(pParent, { 160, 120 }, { 4, 4 })
//{
//	sAppName = "wxWidgets Mouse Demo";
//}
//
//bool MouseFollowPanel::OnUserCreate()
//{
//	return true;
//}
//
//bool MouseFollowPanel::OnUserUpdate(float fElapsedTime)
//{
//	olc_IgnoreUnused(fElapsedTime);
//	auto& draw = GetDraw();
//	draw.Clear(olc::Colour::DARK_GREEN);
//	draw.FilledCircle(GetMouse().GetPosition(), 6.0f, olc::Colour::RED);
//	draw.String({ 4, 4 }, "Move mouse here", olc::Colour::WHITE);
//	return true;
//}

// === cMain ===

cMain::cMain()
	: wxFrame(nullptr, wxID_ANY, "olcPGE3 wxWidgets", wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE)
{
	wxPanel* pRoot = new wxPanel(this);
	wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pBall = new BallPanel(pRoot);
	//m_pMouse = new MouseFollowPanel(pRoot);

	pSizer->Add(m_pBall, 1, wxEXPAND | wxALL, 8);
	//pSizer->Add(m_pMouse, 1, wxEXPAND | wxALL, 8);
	pRoot->SetSizer(pSizer);

	// Drive redraws via a timer, since "only cause a redraw when the
	// panel is refreshed" means the application - not the engine - is
	// responsible for frame pacing.
	m_timer.Bind(wxEVT_TIMER, &cMain::OnTick, this);
	m_timer.Start(1000 / 60); // ~60 FPS
}

void cMain::OnTick(wxTimerEvent& event)
{
	olc_IgnoreUnused(event);
	m_pBall->Refresh(false);
	//m_pMouse->Refresh(false);
}

