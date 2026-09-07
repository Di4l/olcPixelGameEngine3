#pragma once

#include <wx/wx.h>

class cApp : public wxApp
{
public:
	virtual bool OnInit() override;
};

wxDECLARE_APP(cApp);

