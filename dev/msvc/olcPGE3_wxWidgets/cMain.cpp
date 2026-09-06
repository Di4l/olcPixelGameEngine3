#include "cMain.h"


ExamplePanel1::ExamplePanel1(wxWindow* pParent) : olc::wx::PGE3Panel(pParent)
{
	
}

void ExamplePanel1::OnRender()
{
	draw.Line({ 0,0 }, olc::Colour::BLUE, { 256, 240 }, olc::Colour::RED);
	draw.StringProp({ 10, 2 }, "olc::PixelGameEngine\nin\nwxWidgets!", olc::Colour::WHITE);
}






ExamplePanel2::ExamplePanel2(wxWindow* pParent) : olc::wx::PGE3Panel(pParent)
{

}

ExamplePanel2::ExamplePanel2(wxWindow* pParent, const olc::vi2d& vFixedSize) : olc::wx::PGE3Panel(pParent, vFixedSize)
{

}

void ExamplePanel2::OnCreate()
{	

	vPos = { 100,100 };
	vVel = olc::vf2d(100.0f, 1.0f).cart();
}

void ExamplePanel2::OnRender()
{
	draw.Clear(olc::Colour::VERY_DARK_BLUE);
	draw.FilledCircle(vPos.round(), 10, olc::Colour::WHITE);
}

void ExamplePanel2::OnUpdate(const float fElapsedTime)
{
	vPos += vVel * fElapsedTime;

	if (vPos.x - 10 <= 0)
	{
		vPos.x = 10;
		vVel.x *= -1;
	}
	if (vPos.y - 10 <= 0)
	{
		vPos.y = 10;
		vVel.y *= -1;
	}
	if (vPos.x + 10 >= draw.GetTargetSize().x)
	{
		vPos.x = draw.GetTargetSize().x - 10;
		vVel.x *= -1;
	}

	if (vPos.y + 10 >= draw.GetTargetSize().y)
	{
		vPos.y = draw.GetTargetSize().y - 10;
		vVel.y *= -1;
	}
}



cMain::cMain()
	: wxFrame(nullptr, wxID_ANY, "olcPGE3 wxWidgets", wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE)
{

	wxPanel* pRoot = new wxPanel(this);
	wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pPanel1 = new ExamplePanel1(pRoot);
	pSizer->Add(m_pPanel1, 1, wxEXPAND | wxALL, 8);
	
	m_pPanel2 = new ExamplePanel2(pRoot);
	m_pPanel2->OnCreate();
	pSizer->Add(m_pPanel2, 1, wxEXPAND | wxALL, 8);

	m_pPanel3 = new ExamplePanel2(pRoot, { 128,128 });
	m_pPanel3->OnCreate();
	pSizer->Add(m_pPanel3, 1, wxEXPAND | wxALL, 8);

	pRoot->SetSizer(pSizer);

	m_timer.Bind(wxEVT_TIMER, &cMain::OnTick, this);
	m_timer.Start(1000 / 60); 
}

void cMain::OnTick(wxTimerEvent& event)
{
	olc_IgnoreUnused(event);
	m_pPanel1->OnUpdate(1.0f / 60.0f);
	m_pPanel1->Refresh(false);
	
	m_pPanel2->OnUpdate(1.0f / 60.0f);
	m_pPanel2->Refresh(false);

	m_pPanel3->OnUpdate(1.0f / 60.0f);
	m_pPanel3->Refresh(false);
}


