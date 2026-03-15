#pragma once

#include "core.h"

//! START STDHEADER GLOBAL
#include <atomic>
#include <cstdint>
#include <cstring>
#include <thread>
#include <vector>
#include <unordered_map>
//! END STDHEADER

//! START CUSTOMHEADER
#include "host_iface.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(DISABLE_LIBDECOR) || defined(FORCE_WAYLAND_LIBDECOR)
#define ENABLE_LIBDECOR
#endif

#if !defined(FORCE_WAYLAND_LIBDECOR)
#define ENABLE_DECORATION_PROTOCOL
#endif

#if !defined(ENABLE_LIBDECOR) && !defined(ENABLE_DECORATION_PROTOCOL)
#error "Incorrect build configuration.  Either xdg-decoration or libdecor (or both) must be enabled."
#endif

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <wayland-egl.h>
#include "xdg-shell.h"

// Only include the decoration protocol if we are not forcing libdecor
#ifdef ENABLE_DECORATION_PROTOCOL
#include "xdg-decoration.h"
#endif

#include "pointer-warp.h"
#include <linux/input-event-codes.h>
#include <xkbcommon/xkbcommon.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>

#ifdef ENABLE_LIBDECOR
#include "libdecor.h"
#endif

#include <EGL/egl.h>
#include <EGL/eglplatform.h>

#ifndef WL_KEYBOARD_KEY_STATE_REPEATED
#define WL_KEYBOARD_KEY_STATE_REPEATED 2
#endif

namespace olc::host
{
    struct WaylandWindow  {
        wl_surface* surface{nullptr};
        xdg_surface* surface_xdg{nullptr};
        xdg_toplevel* toplevel{nullptr};
        #ifdef ENABLE_DECORATION_PROTOCOL
        zxdg_toplevel_decoration_v1* decorations{nullptr};
        #endif
        wl_egl_window* window{nullptr};
        size_t olc_window_uid{0};
        int32_t bounds_x{0};
        int32_t bounds_y{0};
        bool cursor_visible{true};
        // Ignore window size bounds for fullscreen events
        bool fullscreen{false};

        #ifdef ENABLE_LIBDECOR
        // libdecor support
        libdecor_frame* decor_frame{nullptr};
        int configured_width{};
        int configured_height{};
        libdecor_window_state decor_window_state;
        int floating_width{};
        int floating_height{};
        #endif
        ~WaylandWindow();
    };

    namespace wayland {
        enum PointerEventMask {
            PointerEventEnter = 1 << 0,
            PointerEventLeave = 1 << 1,
            PointerEventMotion = 1 << 2,
            PointerEventButton = 1 << 3,
            PointerEventAxis = 1 << 4, 
            PointerEventAxisSource = 1 << 5,
            PointerEventAxisStop = 1 << 6,
            PointerEventDiscrete = 1 << 7
        };

        struct PointerState {
            uint32_t event_mask{0};
            wl_surface* surface{nullptr};
            wl_fixed_t surface_x{};
            wl_fixed_t surface_y{};
            uint32_t button{0};
            uint32_t state{0};
            uint32_t time{0};
            uint32_t serial{0};

            struct Axis {
                wl_fixed_t value{};
                int32_t discrete{0};
                bool valid{false};
            };

            std::array<Axis, 2> axes{};
            uint32_t axis_source{0};
        };
    }

    class Host_Linux_Wayland : public olc::host::Host
    {
	private:
		wl_display* display{nullptr};
        wl_registry* registry{nullptr};
        wl_shm* shm{nullptr};
        wl_compositor* compositor{nullptr};
        wl_seat* seat{nullptr};
        wl_pointer* pointer{nullptr};
        wl_keyboard* keyboard{nullptr};
        uint32_t keyboard_version{0};
        xkb_context* kb_context{nullptr};
        xkb_state* kb_state{nullptr};
        xkb_keymap* kb_keymap{nullptr};
        uint32_t kb_group{0};
        xdg_wm_base* xdg_wm{nullptr};
        #ifdef ENABLE_DECORATION_PROTOCOL
        zxdg_decoration_manager_v1* decoration_manager{nullptr};
        #endif
        wp_pointer_warp_v1* pointer_warp{nullptr};
        uint32_t enter_serial{0};
        
        wayland::PointerState pointer_state;
        wl_surface* cursor_surface{nullptr};
        wl_cursor_image* cursor_image{nullptr};
        wl_cursor_theme* cursor_theme{nullptr};

        size_t active_window_id;
        
        #ifdef ENABLE_LIBDECOR
        // libdecor support
        bool using_libdecor{false};
        libdecor* decor_context{nullptr};
        std::mutex decor_mutex;
        #endif

    public:
        Host_Linux_Wayland();
        ~Host_Linux_Wayland();

        bool AddWindowFrame(olc::Window* pWindow, const olc::vi2d& vWindowPos, const olc::vi2d& vWindowSize, const bool bFullScreen) override;
        bool CloseWindowFrame(olc::Window* pWindow) override;
        bool UpdateWindowFrameTitle(olc::Window* pWindow) override;

        std::vector<void*> GetHostWindowDescriptor(olc::Window* pWindow) override;

        olc::KeyboardLayout GetKeyboardLayout() const override;
        void UpdateKeyboardLayout();

        // Wait for entire host desktop refresh (for smooooth vsync)
        bool SyncWithDesktopComposite() override;
        bool SetMousePosition(olc::Window* pWindow, const olc::vi2d& vPos) override;
        bool SetMouseVisible(olc::Window* pWindow, const bool bVisible) override;
        bool SetFullScreen(olc::Window* pWindow, const bool bFullScreen) override;

    public:
        bool OnApplicationStart(olc::PixelGameEngine* pPrimary) override;
        bool StartSystem() override;
        bool StopSystem() override;
        bool OnSystemThreadStart() override;
        bool OnSystemTick() override;
        bool OnSystemThreadEnd() override;
        bool OnApplicationEnd() override;
    
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
        static void pointer_frame_callback(void* data, wl_pointer* pointer);
        static void pointer_axis_source_callback(void* data, wl_pointer* pointer, uint32_t axis_source);
        static void pointer_axis_stop_callback(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis);
        static void pointer_axis_discrete_callback(void* data, wl_pointer* pointer, uint32_t axis, int32_t discrete);
        static void pointer_axis_value120_callback(void* data, wl_pointer* pointer, uint32_t axis, int32_t value120);
        static void pointer_axis_relative_direction_callback(void* data, wl_pointer* pointer, uint32_t axis, uint32_t direction);

        // Keyboard callbacks
        static void keyboard_keymap_callback(void* data, wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size);
        static void keyboard_enter_callback(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys);
        static void keyboard_leave_callback(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface);
        static void keyboard_key_callback(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
        static void keyboard_modifiers_callback(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
        static void keyboard_repeat_info_callback(void* data, wl_keyboard* keyboard, int32_t rate, int32_t delay);

        // xdg callbacks
        static void xdg_wm_ping_callback(void* data, xdg_wm_base* wm, uint32_t serial);
        static void xdg_surface_configure_callback(void* data, xdg_surface* surface, uint32_t serial);
        static void xdg_toplevel_configure_callback(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states);
        static void xdg_toplevel_close_callback(void* data, xdg_toplevel* toplevel);
        static void xdg_toplevel_configure_bounds_callback(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height);
        static void xdg_toplevel_capabilities_callback(void* data, xdg_toplevel* toplevel, wl_array* capabilities);
        #ifdef ENABLE_DECORATION_PROTOCOL
        static void xdg_toplevel_decoration_configure_callback(void* data, zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1, uint32_t mode);
        #endif

        #ifdef ENABLE_LIBDECOR
        // libdecor callbacks
        static void libdecor_error_callback(libdecor* context, libdecor_error error, const char* message);
        static void libdecor_frame_configure_callback(libdecor_frame* frame, libdecor_configuration* config, void* data);
        static void libdecor_close_callback(libdecor_frame* frame, void* data);
        static void libdecor_commit_callback(libdecor_frame* frame, void* data);
        static void libdecor_dismiss_popup_callback(libdecor_frame* frame, const char* seat_name, void* data);
        #endif

    private:
        // Wayland callback functions
        void registry_handle_global(wl_registry* registry, uint32_t name, const char* interface, uint32_t version);
        void registry_handle_global_remove(wl_registry* registry, uint32_t name);
        void seat_capabilities(wl_seat* seat, uint32_t capabilities);

        // Pointer callback functions
        void pointer_enter(wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
        void pointer_leave(wl_pointer* pointer, uint32_t serial, wl_surface* surface);
        void pointer_motion(wl_pointer* pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);
        void pointer_button(wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
        void pointer_axis(wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
        void pointer_frame(wl_pointer* pointer);
        void pointer_axis_source(wl_pointer* pointer, uint32_t axis_source);
        void pointer_axis_stop(wl_pointer* pointer, uint32_t time, uint32_t axis);
        void pointer_axis_discrete(wl_pointer* pointer, uint32_t axis, int32_t discrete);
        void pointer_axis_value120(wl_pointer* pointer, uint32_t axis, int32_t value120);
        void pointer_axis_relative_direction(wl_pointer* pointer, uint32_t axis, uint32_t direction);

        // Keyboard Callback Functions
        void keyboard_keymap(wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size);
        void keyboard_enter(wl_keyboard* keyboard, uint32_t serial, wl_surface* surface, wl_array* keys);
        void keyboard_leave(wl_keyboard* keyboard, uint32_t serial, wl_surface* surface);
        void keyboard_key(wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
        void keyboard_modifiers(wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);

        // XDG toplevel callback functions
        void xdg_toplevel_configure(xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states);
        void xdg_toplevel_close(xdg_toplevel* toplevel);
        void xdg_toplevel_configure_bounds(xdg_toplevel* toplevel, int32_t width, int32_t height);

        #ifdef ENABLE_LIBDECOR
        // libdecor callback functions
        void libdecor_frame_configure(libdecor_frame* frame, libdecor_configuration* config);
        void libdecor_close(libdecor_frame* frame);
        void libdecor_commit(libdecor_frame* frame);
        #endif

        bool CreateEGLContext(WaylandWindow* window);

        std::unordered_map<size_t, WaylandWindow> mapUID2Window;
        std::unordered_map<size_t, olc::Window*> mapUID2OlcWindow;
        std::atomic<bool> systemActive {true};
        std::unordered_map<uint32_t, olc::Key> mapKeys;
        olc::KeyboardLayout keyboardLayout{OLC_DEFAULT_KEYBOARD_LAYOUT};
    };
}

//! END DECLARATION