#pragma once

//! START STDHEADER GLOBAL
#include <atomic>
#include <cstdint>
#include <cstring>
#include <vector>
#include <unordered_map>
//! END STDHEADER

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER

//! START DECLARATION
#include <wayland-client.h>
#include <wayland-egl.h>
#include "xdg-shell.h"

#include <EGL/egl.h>
#include <EGL/eglplatform.h>

namespace olc::host
{
    struct WaylandWindow  {
        wl_surface* surface;
        xdg_surface* surface_xdg;
        xdg_toplevel* toplevel;
        //wl_shell* shell;
        //wl_shell_surface* shell_surface;
        wl_egl_window* window;
        size_t olc_window_uid;
    };

    class Host_Linux_Wayland : public olc::host::Host
    {
	private:
		wl_display* display{nullptr};
        wl_registry* registry{nullptr};
        wl_compositor* compositor{nullptr};
        wl_seat* seat{nullptr};
        wl_pointer* pointer{nullptr};
        //wl_surface* surface;
        //wl_shell* shell;
        xdg_wm_base* xdg_wm{nullptr};
        //wl_shell_surface* shell_surface;

    public:
        Host_Linux_Wayland();
        ~Host_Linux_Wayland();

        bool StartSystemEventLoop(bool bBlockIfPossible = false) override;
        bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
        bool CloseWindowFrame(olc::Window* pWindow) override;
        bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

        std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;
        
        
        bool ConnectHostResourceToRenderer() override;

        // Wait for entire host desktop refresh (for smooooth vsync)
        bool SyncWithDesktopComposite() override;

        // Various callbacks from the wayland protocol
        static void registry_handle_global_callback(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version);
        static void registry_handle_global_remove_callback(void* data, wl_registry* registry, uint32_t name);

        static void seat_capabilities_callback(void* data, wl_seat* seat, uint32_t capabilities);
        static void seat_name_callback(void* data, wl_seat* wl_seat, const char* name);

        // pointer callbacks
        static void pointer_enter_callback(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
        static void pointer_leave_callback(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface);
        static void pointer_motion_callback(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);
        static void pointer_button_callback(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
        static void pointer_axis_callback(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
        static void pointer_axis_source(void* data, wl_pointer* pointer, uint32_t axis_source);
        static void pointer_axis_stop(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis);
        static void pointer_axis_discrete(void* data, wl_pointer* pointer, uint32_t axis, int32_t discrete);

        // xdg callbacks
        static void xdg_wm_ping_callback(void* data, xdg_wm_base* wm, uint32_t serial);
        static void xdg_surface_configure_callback(void* data, xdg_surface* surface, uint32_t serial);
        static void xdg_toplevel_configure_callback(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states);
        static void xdg_toplevel_close_callback(void* data, xdg_toplevel* toplevel);
        static void xdg_toplevel_configure_bounds_callback(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height);
        static void xdg_toplevel_capabilities(void* data, xdg_toplevel* toplevel, wl_array* capabilities);
    private:
        void registry_handle_global(wl_registry* registry, uint32_t name, const char* interface, uint32_t version);
        void registry_handle_global_remove(wl_registry* registry, uint32_t name);
        void seat_capabilities(wl_seat* seat, uint32_t capabilities);
        void xdg_toplevel_configure(xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states);
        void xdg_toplevel_close(xdg_toplevel* toplevel);

        bool CreateEGLContext(WaylandWindow* window);

        std::unordered_map<size_t, WaylandWindow> mapUID2Window;
        std::unordered_map<size_t, olc::Window*> mapUID2OlcWindow;
        std::atomic<bool> terminate {false};
    };
}

//! END DECLARATION