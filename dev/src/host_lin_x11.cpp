#include "host_lin_x11.h"

//! START IMPLEMENTATION
namespace olc::host
{
    Host_Linux_X11::Host_Linux_X11()
    {
        using namespace X11;
        XInitThreads();
        olc_Display = XOpenDisplay(NULL);
        olc_WindowRoot = DefaultRootWindow(olc_Display);
    }

    bool Host_Linux_X11::StartSystemEventLoop(bool bBlockIfPossible)
    {
        using namespace X11;

        auto get_pge_window = [&](auto x11_window) -> olc::Window* {
            auto itr = mapX11Window2PTR.find(x11_window);
            if(itr != mapX11Window2PTR.end()) {
                return itr->second;
            }
            return nullptr;
        };

        //std::unordered_map<X11::Window, olc::Window*> mapX11Window2PTR;

        if(bBlockIfPossible) {
			X11::XEvent xev;
            while(!terminate){
                while (XPending(olc_Display))
                {
                    XNextEvent(olc_Display, &xev);

                    if (xev.type == Expose)
                    {
                        //auto* expose_event = reinterpret_cast<XExposeEvent*>(&xev);
                        X11::XExposeEvent& e = xev.xexpose;
                        if(auto* pge_window = get_pge_window(e.window); pge_window) {
                            X11::XWindowAttributes gwa;
                            X11::XGetWindowAttributes(e.display, e.window, &gwa);
                            pge_window->olc_OnWindowSize(olc::vi2d{gwa.width, gwa.height});
                        }
                    }
                    else if (xev.type == ConfigureNotify)
                    {
                        X11::XConfigureEvent& xce = xev.xconfigure;
                        if(auto* pge_window = get_pge_window(xce.window); pge_window) {
                            pge_window->olc_OnWindowSize(olc::vi2d{xce.width, xce.height});
                        }
                    }
                    // else if (xev.type == KeyPress)
                    // {
                    // 	KeySym ks;

                    // 	// DragonEye still loves numpads, but this is a better way
                    // 	XLookupString(&xev.xkey, NULL, 0, &ks, NULL);

                    // 	if (ks != NoSymbol)
                    // 		ptrPGE->olc_UpdateKeyState(ks, true);
                    // }
                    // else if (xev.type == KeyRelease)
                    // {
                    // 	KeySym ks;
                    // 	XLookupString(&xev.xkey, NULL, 0, &ks, NULL);

                    // 	if (ks != NoSymbol)
                    // 		ptrPGE->olc_UpdateKeyState(ks, false);
                    // }
                    else if (xev.type == ButtonPress)
                    {
                        if(auto* pge_window = get_pge_window(xev.xbutton.window); pge_window) {
                            switch (xev.xbutton.button)
                            {
                            case 1:	pge_window->olc_OnMouseButton(0, true); break;
                            case 2:	pge_window->olc_OnMouseButton(2, true); break;
                            case 3:	pge_window->olc_OnMouseButton(1, true); break;
                            case 4:	pge_window->olc_OnMouseWheel(120); break;
                            case 5:	pge_window->olc_OnMouseWheel(-120); break;
                            default: break;
                            }
                        
                        }
                    }
                    else if (xev.type == ButtonRelease)
                    {
                        if(auto* pge_window = get_pge_window(xev.xbutton.window); pge_window) {
                            switch (xev.xbutton.button)
                            {
                            case 1:	pge_window->olc_OnMouseButton(0, false); break;
                            case 2:	pge_window->olc_OnMouseButton(2, false); break;
                            case 3:	pge_window->olc_OnMouseButton(1, false); break;
                            default: break;
                            }               
                        }
                    }
                    else if (xev.type == MotionNotify)
                    {
                        X11::XMotionEvent& xme = xev.xmotion;
                        if(auto* pge_window = get_pge_window(xev.xbutton.window); pge_window) {
                            pge_window->olc_OnMouseMove(olc::vi2d{xme.x, xme.y});
                        
                        }
                    }
                    // else if (xev.type == FocusIn)
                    // {
                    // 	ptrPGE->olc_UpdateKeyFocus(true);
                    // }
                    // else if (xev.type == FocusOut)
                    // {
                    // 	ptrPGE->olc_UpdateKeyFocus(false);
                    // }
                    else if (xev.type == ClientMessage)
                    {
                        X11::XClientMessageEvent& xcme = xev.xclient;
                        if(auto* pge_window = get_pge_window(xcme.window); pge_window) {
                            pge_window->olc_OnWindowClose();
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    bool Host_Linux_X11::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
    {
		// The user created olc::Window object is the SSoT for what a window
		// should look like, so get that sort of thing from there
		olc::vi2d vWinPos = vWindowPos;
		olc::vi2d vWinSize = vWindowSize;

        // Based on the display capabilities, configure the appearance of the window
        // to do this namespacing, both x11 and glx have to be included in the x11 namespace
        GLint olc_GLAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        olc_VisualInfo = glXChooseVisual(olc_Display, 0, olc_GLAttribs);
        olc_ColourMap = XCreateColormap(olc_Display, olc_WindowRoot, olc_VisualInfo->visual, AllocNone);
        olc_SetWindowAttribs.colormap = olc_ColourMap;

        // Register which events we are interested in receiving
        olc_SetWindowAttribs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | StructureNotifyMask;

        // Create the window
        X11::Window olc_Window = XCreateWindow(olc_Display, olc_WindowRoot, vWindowPos.x, vWindowPos.y,
            vWindowSize.x, vWindowSize.y,
            0, olc_VisualInfo->depth, InputOutput, olc_VisualInfo->visual,
            CWColormap | CWEventMask, &olc_SetWindowAttribs);

            
        X11::Atom wmDelete = XInternAtom(olc_Display, "WM_DELETE_WINDOW", true);
        X11::XSetWMProtocols(olc_Display, olc_Window, &wmDelete, 1);
        
        XMapWindow(olc_Display, olc_Window);
        XStoreName(olc_Display, olc_Window, "OneLoneCoder.com - Pixel Game Engine");
            pWindow->SetWindowSize(vWindowSize);
            
        mapUID2X11Window.insert_or_assign(pWindow->GetUID(), olc_Window);
		mapX11Window2PTR.insert_or_assign(olc_Window, pWindow);

        return true;
    }

    bool Host_Linux_X11::CloseWindowFrame(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2X11Window.find(pWindow->GetUID());
        if (window_handle != mapUID2X11Window.end()) {
            X11::XDestroyWindow(olc_Display, window_handle->second);
            mapUID2X11Window.erase(window_handle);
        }
        return true;
    }
    
    bool Host_Linux_X11::UpdateWindowFrameTitle(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2X11Window.find(pWindow->GetUID());
        if (window_handle != mapUID2X11Window.end()) {
            X11::XStoreName(olc_Display, window_handle->second, pWindow->GetWindowTitle().c_str());
        }
        return true;
    }

    std::vector<void*> Host_Linux_X11::GetHostWindowDescriptor(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2X11Window.find(pWindow->GetUID());
        if (window_handle != mapUID2X11Window.end()) {
            return {reinterpret_cast<void*>(window_handle->second),
                reinterpret_cast<void*>(olc_Display)
            };
        }
  		return {};
    }
    
    
    bool Host_Linux_X11::ConnectHostResourceToRenderer()
    {
        return true;
    }

    // Wait for entire host desktop refresh (for smooooth vsync)
    bool Host_Linux_X11::SyncWithDesktopComposite()
    {
        return true;
    }
}
//! END IMPLEMENTATION
