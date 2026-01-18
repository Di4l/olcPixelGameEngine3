#include "host_lin_wayland.h"


//! START IMPLEMENTATION
namespace olc::host
{
    namespace wayland {
        static const wl_registry_listener registry_listener {
            .global = Host_Linux_Wayland::registry_handle_global_callback,
            .global_remove = Host_Linux_Wayland::registry_handle_global_remove_callback,
        };

        static const wl_seat_listener seat_listener {
            .capabilities = Host_Linux_Wayland::seat_capabilities_callback,
            .name = Host_Linux_Wayland::seat_name_callback
        };

        static const wl_pointer_listener pointer_listener {

        };
    }

    namespace xdg {
        static const xdg_wm_base_listener xdg_base_listener {
            .ping = Host_Linux_Wayland::xdg_wm_ping_callback
        };

        static const xdg_surface_listener surface_listener {
            .configure = Host_Linux_Wayland::xdg_surface_configure_callback
        };

        static const xdg_toplevel_listener xdg_top_listener {
            .configure = Host_Linux_Wayland::xdg_toplevel_configure_callback,
            .close = Host_Linux_Wayland::xdg_toplevel_close_callback,
            .configure_bounds = Host_Linux_Wayland::xdg_toplevel_configure_bounds_callback,
            .wm_capabilities = Host_Linux_Wayland::xdg_toplevel_capabilities
        };
    }

    Host_Linux_Wayland::Host_Linux_Wayland()
    {
        
        display = wl_display_connect(NULL);
        registry = wl_display_get_registry(display);

        wl_registry_add_listener(registry, &wayland::registry_listener, this);
        wl_display_roundtrip(display);

        if(compositor == nullptr || xdg_wm == nullptr || seat == nullptr) {
            throw;
        }
        
        xdg_wm_base_add_listener(xdg_wm, &xdg::xdg_base_listener, this);
        wl_seat_add_listener(seat, &wayland::seat_listener, this);


        //surface = static_cast<wl_surface*>(wl_compositor_create_surface(compositor));
        //shell_surface = static_cast<wl_shell_surface*>(wl_shell_get_shell_surface(shell, surface));
        //wl_shell_surface_set_toplevel(shell_surface);
    }

    Host_Linux_Wayland::~Host_Linux_Wayland()
    {
        for (auto& itr : mapUID2Window) {
            auto& wayland_window = itr.second;
            wl_egl_window_destroy(wayland_window.window);
            xdg_toplevel_destroy(wayland_window.toplevel);
            xdg_surface_destroy(wayland_window.surface_xdg);
            wl_surface_destroy(wayland_window.surface);
            //auto uid = wayland_window.olc_window_uid;
            //mapUID2Window.erase(uid);
            //mapUID2OlcWindow.erase(uid);
        }

        wl_display_disconnect(display);
    }

    bool Host_Linux_Wayland::StartSystemEventLoop(bool bBlockIfPOssible)
    {
        if(bBlockIfPOssible) {
            while(!terminate && wl_display_dispatch_pending(display) != -1) {

            }
        }

        return true;
    }

    bool Host_Linux_Wayland::AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen)
    {
        std::cout << __FUNCTION__ << std::endl;

        // Create a window
        WaylandWindow w;
        wl_region* region = wl_compositor_create_region(compositor);
        wl_region_add(region, vWindowPos.x, vWindowPos.y, vWindowSize.x, vWindowSize.y);
        
        w.surface = wl_compositor_create_surface(compositor);
        w.surface_xdg = xdg_wm_base_get_xdg_surface(xdg_wm, w.surface);

        
        //wl_seat_get_pointer()
        //wl_pointer_set_cursor()

        xdg_surface_add_listener(w.surface_xdg, &xdg::surface_listener, this);
        w.toplevel = xdg_surface_get_toplevel(w.surface_xdg);
        xdg_toplevel_set_title(w.toplevel, "Test");
        xdg_toplevel_add_listener(w.toplevel, &xdg::xdg_top_listener, this);
        wl_surface_set_opaque_region(w.surface, region);
        w.window = wl_egl_window_create(w.surface, vWindowSize.x, vWindowSize.y);
        w.olc_window_uid = pWindow->GetUID();
        wl_surface_commit(w.surface);
        wl_region_destroy(region);

        pWindow->SetWindowPosition(vWindowPos);
        pWindow->SetWindowSize(vWindowSize);

        mapUID2Window.insert_or_assign(pWindow->GetUID(), w);
        mapUID2OlcWindow.insert_or_assign(pWindow->GetUID(), pWindow);
        return true;
    }

    bool Host_Linux_Wayland::CloseWindowFrame(olc::Window* pWindow)
    {
        auto itr = mapUID2Window.find(pWindow->GetUID());
        if(itr != mapUID2Window.end()) {
            auto& wayland_window = itr->second;
            wl_egl_window_destroy(wayland_window.window);
            xdg_toplevel_destroy(wayland_window.toplevel);
            xdg_surface_destroy(wayland_window.surface_xdg);
            wl_surface_destroy(wayland_window.surface);
            auto uid = wayland_window.olc_window_uid;
            mapUID2Window.erase(uid);
            mapUID2OlcWindow.erase(uid);
        }

        return true;
    }
    bool Host_Linux_Wayland::UpdateWindowFrameTitle(olc::Window* pWindow)
    {
        auto itr = mapUID2Window.find(pWindow->GetUID());
        if(itr != mapUID2Window.end()) {
            xdg_toplevel_set_title(itr->second.toplevel, pWindow->GetWindowTitle().c_str());
        }
        return true;
    }

    void Host_Linux_Wayland::registry_handle_global(wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
    {
        printf("interface: '%s', version: %d, name: %d\n", interface, version, name);

        if(std::strcmp(interface, wl_compositor_interface.name) == 0) {
            compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, version));
        }
        if(std::strcmp(interface, xdg_wm_base_interface.name) == 0) {
            xdg_wm = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, version));
        }
        if(std::strcmp(interface, wl_seat_interface.name) == 0) {
            seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, version));
        }
    }
    
    void Host_Linux_Wayland::registry_handle_global_remove(wl_registry* registry, uint32_t name)
    {

    }

    void Host_Linux_Wayland::seat_capabilities(wl_seat* seat, uint32_t capabilities)
    {
        if (capabilities & WL_SEAT_CAPABILITY_POINTER && pointer == nullptr) {
            pointer = wl_seat_get_pointer(seat);
            wl_pointer_add_listener(pointer, &wayland::pointer_listener, this);
        }
    }

    void Host_Linux_Wayland::xdg_toplevel_configure(xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states)
    {
        std::cout << __FUNCTION__ << std::endl;
        for(auto& i : mapUID2Window) {
            if(i.second.toplevel == toplevel) {
                wl_egl_window_resize(i.second.window, width, height, 0, 0);
                wl_surface_commit(i.second.surface);
            }
        }
    }

    void Host_Linux_Wayland::xdg_toplevel_close(xdg_toplevel* toplevel)
    {
        std::cout << __FUNCTION__ << std::endl;
        for(auto& i : mapUID2Window) {
            if(i.second.toplevel == toplevel) {
                auto itr = mapUID2OlcWindow.find(i.second.olc_window_uid);
                if(itr != mapUID2OlcWindow.end()) {
                    auto* ptr = itr->second;
                    ptr->olc_OnWindowClose();
                    std::cout << __FUNCTION__;
                    terminate = true;
                }
            }
        }
    }

    void Host_Linux_Wayland::registry_handle_global_callback(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->registry_handle_global(registry, name, interface, version);
    }
    void Host_Linux_Wayland::registry_handle_global_remove_callback(void* data, wl_registry* registry, uint32_t name)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->registry_handle_global_remove(registry, name);
    }

    void Host_Linux_Wayland::seat_capabilities_callback(void* data, wl_seat* seat, uint32_t capabilities)
    {
        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->seat_capabilities(seat, capabilities);
    }

    void Host_Linux_Wayland::seat_name_callback(void* data, wl_seat* wl_seat, const char* name)
    {

    }

    void Host_Linux_Wayland::xdg_wm_ping_callback(void* data, xdg_wm_base* wm, uint32_t serial) {
        std::cout << __FUNCTION__ << std::endl;

        xdg_wm_base_pong(wm, serial);
    }

    void Host_Linux_Wayland::xdg_surface_configure_callback(void* data, xdg_surface* surface, uint32_t serial)
    {
        std::cout << __FUNCTION__ << std::endl;

        xdg_surface_ack_configure(surface, serial);
    }
    void Host_Linux_Wayland::xdg_toplevel_configure_callback(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states)
    {
        std::cout << __FUNCTION__ << std::endl;

        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->xdg_toplevel_configure(toplevel, width, height, states);
    }
    void Host_Linux_Wayland::xdg_toplevel_close_callback(void* data, xdg_toplevel* toplevel)
    {
        std::cout << __FUNCTION__ << std::endl;

        auto* host = reinterpret_cast<Host_Linux_Wayland*>(data);
        host->xdg_toplevel_close(toplevel);
    }

    void Host_Linux_Wayland::xdg_toplevel_configure_bounds_callback(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height) {
        std::cout << __FUNCTION__ << std::endl;
        
        return;
    }

    void Host_Linux_Wayland::xdg_toplevel_capabilities(void* data, xdg_toplevel* toplevel, wl_array* capabilities)
    {
        std::cout << __FUNCTION__ << std::endl;

        return;
    }

    std::vector<void*> Host_Linux_Wayland::GetHostWindowDescriptor(olc::Window* pWindow)
    {
        const auto window_handle = mapUID2Window.find(pWindow->GetUID());
        if (window_handle != mapUID2Window.end()) {
            return {reinterpret_cast<void*>(&window_handle->second),
                reinterpret_cast<void*>(display)
            };
        }
        return {};
    }

    bool Host_Linux_Wayland::ConnectHostResourceToRenderer()
    {
        return true;
    }

    bool Host_Linux_Wayland::SyncWithDesktopComposite()
    {
        return true;
    }

}