#pragma once

#define OLC_USE_WXWIDGETS
#include "..\..\..\olcPixelGameEngine3.h"

class BallPanel : public olc::wx::PGE3Panel
{
public:
	BallPanel(wxWindow* pParent);

protected:
	void OnRender() override;

private:
	olc::vf2d vPos;
	olc::vf2d vVel;
};

class cMain : public wxFrame
{
public:
	cMain();

private:
	void OnTick(wxTimerEvent& event);


private:
	BallPanel* m_pBall1 = nullptr;
	BallPanel* m_pBall2 = nullptr;
	wxTimer m_timer;
};

