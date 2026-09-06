#pragma once

#define OLC_USE_WXWIDGETS
#include "..\..\..\olcPixelGameEngine3.h"

class ExamplePanel1 : public olc::wx::PGE3Panel
{
public:
	ExamplePanel1(wxWindow* pParent);

protected:
	void OnRender() override;

};


class ExamplePanel2 : public olc::wx::PGE3Panel
{
public:
	ExamplePanel2(wxWindow* pParent);
	ExamplePanel2(wxWindow* pParent, const olc::vi2d& vFixedSize);

public:
	void OnUpdate(const float fElapsedTime) override;
	void OnCreate() override;
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
	ExamplePanel1* m_pPanel1 = nullptr;
	ExamplePanel2* m_pPanel2 = nullptr;
	ExamplePanel2* m_pPanel3 = nullptr;
	wxTimer m_timer;
};

