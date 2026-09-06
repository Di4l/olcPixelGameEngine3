#pragma once

#define OLC_USE_WXWIDGETS
#include "..\..\..\olcPixelGameEngine3.h"




// A simple bouncing-ball panel
class BallPanel : public olc::wx::PGE3Panel
{
public:
	BallPanel(wxWindow* pParent, olc::wx::PGE3Core* core = nullptr);

private:
	olc::vf2d vPos;
	olc::vf2d vVel;
};

// A simple mouse-follow panel to demonstrate independent input per-panel
//class MouseFollowPanel : public olc::wx::wxOLCPGE3Panel
//{
//public:
//	MouseFollowPanel(wxWindow* pParent);
//
//	bool OnUserCreate() override;
//	bool OnUserUpdate(float fElapsedTime) override;
//};

class cMain : public wxFrame
{
public:
	cMain();

private:
	void OnTick(wxTimerEvent& event);

private:
	BallPanel* m_pBall = nullptr;
	//MouseFollowPanel* m_pMouse = nullptr;
	wxTimer m_timer;
};

