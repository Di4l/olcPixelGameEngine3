#include "host_win_winapi.h"

//! START IMPLEMENTATION
namespace olc::host
{
	// Forward Declaration
	static LRESULT CALLBACK WINAPI_EventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Windows app needs an event loop somewhere. This is blocking of course. This loop handles
	// all windows created for this host.
	bool Host_Windows_WinAPI::StartSystemEventLoop()
	{
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return true;
	}

	// Static linkage to lpfnWndProc - the hWnd is tagged with meta-info to get
	// access to the actual host instance, which can more conveninetly process
	// the event across multiple window instances
	static LRESULT CALLBACK WINAPI_EventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// CreateWindowEx will fire a WM_CREATE event at the window, which we're
		// not interested in, and this will occur before we've populated our 
		// linkage maps. We can detect for this condition here which means
		// subsequent look-ups dont fail. NOTE: Do not assume WM_CREATE is the
		// first message that is sent, its just one that we know is reliably sent.
		if (uMsg == WM_CREATE)
		{
			// Associate the window's little blob of user memory with host iface
			auto cfg = ((CREATESTRUCT*)lParam)->lpCreateParams;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)cfg);
		}
		else
		{
			// If this returns a value, then the host iface should be responsible
			// for handling the message.
			auto host = (Host_Windows_WinAPI*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (host)
				return host->OnWindowEvent(hWnd, uMsg, wParam, lParam);
		}
		
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	std::wstring Host_Windows_WinAPI::ConvertS2W(std::string s)
	{
#ifdef __MINGW32__
		wchar_t* buffer = new wchar_t[s.length() + 1];
		mbstowcs(buffer, s.c_str(), s.length());
		buffer[s.length()] = L'\0';
#else
		int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
		wchar_t* buffer = new wchar_t[count];
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);
#endif
		std::wstring w(buffer);
		delete[] buffer;
		return w;
	}

	bool Host_Windows_WinAPI::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
	{
		// The user created olc::Window object is the SSoT for what a window
		// should look like, so get that sort of thing from there
		olc::vi2d vWinPos = vWindowPos;
		olc::vi2d vWinSize = vWindowSize;
		

		// Define WindowClass
		WNDCLASS wc = { 0 };
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpfnWndProc = WINAPI_EventHandler;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;// sizeof(this); // For static meta-info
		wc.lpszMenuName = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszClassName = olcT("OLC_PIXEL_GAME_ENGINE3");
		RegisterClass(&wc);

		// Define window furniture
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;

		olc::vi2d vTopLeft = vWindowPos;

		//// Handle Fullscreen
		//if (bFullScreen)
		//{
		//	dwExStyle = 0;
		//	dwStyle = WS_VISIBLE | WS_POPUP;
		//	HMONITOR hmon = MonitorFromWindow(olc_hWnd, MONITOR_DEFAULTTONEAREST);
		//	MONITORINFO mi = { sizeof(mi) };
		//	if (!GetMonitorInfo(hmon, &mi)) return olc::rcode::FAIL;
		//	vWindowSize = { mi.rcMonitor.right, mi.rcMonitor.bottom };
		//	vTopLeft.x = 0;
		//	vTopLeft.y = 0;
		//}

		// Keep client size as requested
		RECT rWndRect = { 0, 0, vWindowSize.x, vWindowSize.y };
		AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
		int width = rWndRect.right - rWndRect.left;
		int height = rWndRect.bottom - rWndRect.top;
		pWindow->SetSize(vWindowSize);

		// Create the actual OS window, return a handle
		HWND hWnd = CreateWindowEx(dwExStyle, olcT("OLC_PIXEL_GAME_ENGINE3"), olcT(""), dwStyle,
			vTopLeft.x, vTopLeft.y, width, height, NULL, NULL, GetModuleHandle(nullptr), this);

		SetWindowPos(hWnd, NULL, vWinPos.x, vWinPos.y, width, height, SWP_SHOWWINDOW);

		// Now... awkwardly, the above has already fired off some window messages
		// and they arent necessarily in a consistent order. Whereas one might 
		// assume WM_CREATE would be the first, there are some others on more
		// modern systems. This is awkward because we havent yet associated the
		// source window with a long_ptr to this class, and therefore we can't
		// call the appropriate event handler.


		// Store the link bewteen host resource and window
		mapUID2HWND.insert_or_assign(pWindow->GetUID(), hWnd);
		mapHWND2PTR.insert_or_assign(hWnd, pWindow);

		pWindow->ConnecToHost(this);


		//DragAcceptFiles(olc_hWnd, true);

		return true;
	}

	bool Host_Windows_WinAPI::UpdateWindowFrameTitle(olc::Window* pWindow)
	{
#ifdef UNICODE
		SetWindowText(mapUID2HWND.at(pWindow->GetUID()), ConvertS2W(pWindow->GetTitle()).c_str());
#else
		SetWindowText(mapUID2HWND.at(pWindow->GetUID()), sTitle.c_str());
#endif
		return true;
	}

	std::vector<void*> Host_Windows_WinAPI::GetHostWindowDescriptor(olc::Window* pWindow)
	{
		return { mapUID2HWND[pWindow->GetUID()] };
	}

	bool Host_Windows_WinAPI::ConnectHostResourceToRenderer()
	{
		return false;
	}

	bool Host_Windows_WinAPI::SyncWithDesktopComposite()
	{
		return DwmFlush() == S_OK;
	}

	LRESULT Host_Windows_WinAPI::OnWindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (!mapHWND2PTR.contains(hWnd))
			return false;

		// Get target olc::Window
		const auto& window = mapHWND2PTR.at(hWnd);

		// Many WinAPI events are literally ancient these days, so need some interpretation
		// to get to the useful data.

		switch (uMsg)
		{
		case WM_MOUSEMOVE: // Mouse has moved within a window
			{
				// Extract mouse X & Y
				uint16_t x = uint16_t(lParam & 0xFFFF); 
				uint16_t y = uint16_t((lParam >> 16) & 0xFFFF);
				int16_t ix = *(int16_t*)&x;   
				int16_t iy = *(int16_t*)&y;
				// Tell window new mouse location
				window->olc_OnMouseMove(olc::vi2d{ ix, iy });
				return 0;
			}
			break;
		

			//		case WM_MOVE:       vWinPos = olc::vi2d(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF);  ptrPGE->olc_UpdateWindowPos(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF);	return 0;
		case WM_SIZE:
			{				
				window->olc_OnWindowSize(olc::vi2d(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF));
				return 0;
			}
			break;
			//		case WM_MOUSEWHEEL:	ptrPGE->olc_UpdateMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));           return 0;
			//		case WM_MOUSELEAVE: ptrPGE->olc_UpdateMouseFocus(false);                                    return 0;
			//		case WM_SETFOCUS:	ptrPGE->olc_UpdateKeyFocus(true);                                       return 0;
			//		case WM_KILLFOCUS:	ptrPGE->olc_UpdateKeyFocus(false);                                      return 0;
			//		case WM_KEYDOWN:	ptrPGE->olc_UpdateKeyState(int32_t(wParam), true);                      return 0;
			//		case WM_KEYUP:		ptrPGE->olc_UpdateKeyState(int32_t(wParam), false);                     return 0;
			//		case WM_SYSKEYDOWN: ptrPGE->olc_UpdateKeyState(int32_t(wParam), true);						return 0;
			//		case WM_SYSKEYUP:	ptrPGE->olc_UpdateKeyState(int32_t(wParam), false);						return 0;
		case WM_LBUTTONDOWN:
			{
				window->olc_OnMouseButton(0, true);
				return 0;
			}
		case WM_LBUTTONUP:
			{
				window->olc_OnMouseButton(0, false);
				return 0;
			}
		case WM_RBUTTONDOWN:
			{
				window->olc_OnMouseButton(1, true);
				return 0;
			}
		case WM_RBUTTONUP:
			{
				window->olc_OnMouseButton(1, false);
				return 0;
			}
		case WM_MBUTTONDOWN:
			{
				window->olc_OnMouseButton(2, true);
				return 0;
			}
		case WM_MBUTTONUP:
			{
				window->olc_OnMouseButton(2, false);
				return 0;
			}
			//		case WM_DROPFILES:
			//		{
			//			// This is all eww...
			//			HDROP drop = (HDROP)wParam;
			//			
			//			uint32_t nFiles = DragQueryFile(drop, 0xFFFFFFFF, nullptr, 0);
			//			std::vector<std::string> vFiles;
			//			for (uint32_t i = 0; i < nFiles; i++)
			//			{
			//				TCHAR dfbuffer[256]{};
			//				uint32_t len = DragQueryFile(drop, i, nullptr, 0);
			//				DragQueryFile(drop, i, dfbuffer, 256);
			//#ifdef UNICODE
			//#ifdef __MINGW32__
			//				char* buffer = new char[len + 1];
			//				wcstombs(buffer, dfbuffer, len);
			//				buffer[len] = '\0';
			//#else
			//				int count = WideCharToMultiByte(CP_UTF8, 0, dfbuffer, -1, NULL, 0, NULL, NULL);
			//				char* buffer = new char[count];
			//				WideCharToMultiByte(CP_UTF8, 0, dfbuffer, -1, buffer, count, NULL, NULL);
			//#endif				
			//				vFiles.push_back(std::string(buffer));
			//				delete[] buffer;
			//#else
			//				vFiles.push_back(std::string(dfbuffer));
			//#endif
			//			}
			//			
			//			// Even more eww...
			//			POINT p; DragQueryPoint(drop, &p);
			//			ptrPGE->olc_DropFiles(p.x, p.y, vFiles);
			//			DragFinish(drop);
			//			return 0;
			//		}
			//		break;
			//			
			//			
			//		case WM_CLOSE:		ptrPGE->olc_Terminate();                                                return 0;
		case WM_DESTROY:	
			PostQuitMessage(0); 
			DestroyWindow(hWnd);

		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

};
//! END IMPLEMENTATION

