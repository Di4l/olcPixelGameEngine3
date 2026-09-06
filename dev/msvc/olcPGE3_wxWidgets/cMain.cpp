#include "cMain.h"


BallPanel::BallPanel(wxWindow* pParent) : olc::wx::PGE3Panel(pParent)
{
	
}

void BallPanel::OnRender()
{
	draw.Line({ 0,0 }, olc::Colour::BLUE, { 256, 240 }, olc::Colour::RED);

	draw.StringProp({ 10, 2 }, "olc::PixelGameEngine\nin\nwxWidgets!", olc::Colour::WHITE);
}





cMain::cMain()
	: wxFrame(nullptr, wxID_ANY, "olcPGE3 wxWidgets", wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE)
{

	wxPanel* pRoot = new wxPanel(this);
	wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pBall1 = new BallPanel(pRoot);
	pSizer->Add(m_pBall1, 1, wxEXPAND | wxALL, 8);
	m_pBall2 = new BallPanel(pRoot);
	pSizer->Add(m_pBall2, 1, wxEXPAND | wxALL, 8);

	pRoot->SetSizer(pSizer);

	m_timer.Bind(wxEVT_TIMER, &cMain::OnTick, this);
	m_timer.Start(1000 / 60); 
}

void cMain::OnTick(wxTimerEvent& event)
{
	olc_IgnoreUnused(event);
	//m_pBall->Refresh(false);
	//m_pMouse->Refresh(false);
}

