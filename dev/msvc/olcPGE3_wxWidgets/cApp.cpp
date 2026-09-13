#include "cApp.h"
#include "cMain.h"



wxIMPLEMENT_APP(cApp);

bool cApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	cMain* pMain = new cMain();
	pMain->Show(true);
	SetTopWindow(pMain);

	return true;
}

