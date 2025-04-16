#ifndef AGUI_H_
#define AGUI_H_

#include "shapes.h"
#include <cassert>
#include <cstdint>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#define TO_RGBA(C) (C >> 24) & 0xFF, (C >> 16) & 0xFF, (C >> 8) & 0xFF, C & 0xFF
#define TO_RGB(C) (C >> 24) & 0xFF, (C >> 16) & 0xFF, (C >> 8) & 0xFF

#define AGUI_ASSERT(x) assert (x);

#define RESQ_N() 7, 6, 5, 4, 3, 2, 1
#define ARG_N(_1, _2, _3, _4, _5, _6, _7, N, ...) N
#define ARG_COUNT_IMPL(...) ARG_N (__VA_ARGS__)
#define ARG_COUNT(...) ARG_COUNT_IMPL (__VA_ARGS__, RESQ_N())

#define DISPATCH_NAME(name, n) name##n
#define DISPATCH_IMPL(name, n) DISPATCH_NAME (name, n)
#define DISPATCH(name, ...) DISPATCH_IMPL (name, ARG_COUNT (__VA_ARGS__)) (__VA_ARGS__)

#define FRAME5(name, x, y, w, h) \
    AGUI::create_frame (name, x, y, w, h);

#define FRAME(...)                 \
    DISPATCH (FRAME, __VA_ARGS__); \
    for (int i = 0; i < 1; i = 1, AGUI::frame_end())

#define BUTTON3(name, x, y) \
    AGUI::create_button (name, x, y);

#define BUTTON(...) \
    DISPATCH (BUTTON, __VA_ARGS__)

namespace AGUI
{
    class Button;
    class Label;
    class Rect;
    struct Widget;
    struct Stackable;

    using color_t                              = uint32_t;
    static constexpr color_t clear             = 0x00000000;
    static constexpr color_t white             = 0xFFFFFFFF;
    static constexpr color_t black             = 0x000000FF;
    static constexpr color_t blue              = 0x0000FFFF;
    static constexpr color_t red               = 0xFF0000FF;
    static constexpr color_t green             = 0x00FF00FF;
    static constexpr color_t yellow            = 0xFFFF00FF;
    static constexpr color_t button_background = 0x474A56FF;
    static constexpr color_t button_border     = 0xc2c2c2FF;
    static constexpr color_t button_hover      = 0x737373FF;
    static constexpr color_t button_click      = 0x5E5E5EFF;
    static constexpr color_t frame_bar         = 0x333333FF;
    static constexpr color_t frame_bg          = 0x2E236CFF;

    struct Style
    {
        std::optional<color_t> bg_frame;
        std::optional<color_t> bd_color;
        std::optional<color_t> hover_color;
        std::optional<color_t> click_color;
        std::optional<color_t> bg_button;
        std::optional<float>   bd_width;
        std::optional<float>   padding;
        Style();
    };

    template <class T>
    inline void merge (std::optional<T>& dst, const std::optional<T>& src)
    {
        if (! dst)
            dst = src;
    }

    inline void merge_style (Style& dst, const Style& src)
    {
        merge (dst.bg_frame, src.bg_frame);
        merge (dst.bd_color, src.bd_color);
        merge (dst.hover_color, src.hover_color);
        merge (dst.click_color, src.click_color);
        merge (dst.bg_button, src.bg_button);
        merge (dst.bd_width, src.bd_width);
        merge (dst.padding, src.padding);
    }

    struct Backend
    {
        virtual ~Backend (void) {}
        virtual void draw_rect (const Rect&, const color_t) const                     = 0;
        virtual void draw_fill_rect (const Rect&, const color_t) const                = 0;
        virtual void begin_clip (const Rect&) const                                   = 0;
        virtual void end_clip (void) const                                            = 0;
        virtual void draw_text (const Vec2&, std::string_view, color_t = white) const = 0;
        virtual void calc_text_size (std::string_view, int* w, int* h) const          = 0;
    };

    struct IO
    {
        Point                    mouse_pos;
        bool                     mouse_down;
        std::shared_ptr<Backend> backend;
        const char*              font_path  = "/home/anthony/Workspace/ui/DroidSans.ttf";
        int                      font_size  = 26;
        color_t                  font_color = white;
        IO() {};
    };

    class Frame
    {
      public:
        enum Opt : uint32_t
        {
            None        = 1 << 0,
            No_move     = 1 << 1,
            No_close    = 1 << 2,
            No_minimize = 1 << 3,
            No_resize   = 1 << 4,
            No_bar      = 1 << 5,
        };
        Frame (std::string name, float x, float y, float w, float h, uint32_t = None, const Style& = {});
        void        draw (void);
        bool        move (void);
        void        resize (void);
        std::string ID (void) const;
        void        add_widget (const std::shared_ptr<Widget>);
        bool        has_widget_id (const std::string&) const;
        void        add_element (std::shared_ptr<Stackable>);

      private:
        uint32_t                                                                      options;
        std::string                                                                   name;
        Point                                                                         position;
        Vec2                                                                          size;
        Rect                                                                          frame_bar;
        Rect                                                                          content;
        Rect                                                                          border;
        Rect                                                                          resize_box;
        Style                                                                         style;
        bool                                                                          minimized          = false;
        bool                                                                          resizing           = false;
        bool                                                                          frame_bar_selected = false;
        bool                                                                          mouse_was_down     = false;
        Point                                                                         mouse_down_pos;
        Vec2                                                                          move_offset;
        Vec2                                                                          resize_offset;
        std::unique_ptr<Button>                                                       close_button;
        std::unique_ptr<Button>                                                       minimize_button;
        std::list<std::shared_ptr<Widget>>                                            widgets;
        std::unordered_map<std::string, std::list<std::shared_ptr<Widget>>::iterator> widget_map;
        std::vector<std::shared_ptr<Stackable>>                                       collection;
    };

    using shared_frame_t = std::shared_ptr<Frame>;
    using weak_frame_t   = std::weak_ptr<Frame>;
    struct Context
    {
        IO                                                                   io;
        Style                                                                style;
        std::weak_ptr<Frame>                                                 focused_frame;
        std::list<shared_frame_t>                                            frames;
        std::list<weak_frame_t>                                              frame_order;
        std::unordered_map<std::string, std::list<shared_frame_t>::iterator> frames_map;
        std::unordered_map<std::string, std::list<weak_frame_t>::iterator>   order_map;
    };

    IO&                    get_io (void);
    const AGUI::Style&     get_style (void);
    void                   set_backend (std::unique_ptr<Backend>);
    void                   update (void);
    std::shared_ptr<Frame> create_frame (std::string name, float x, float y, float w, float h, uint32_t optison = Frame::None, const Style& = {});
    Button*                create_button (std::string label, float x, float y);
    Button*                create_button (std::string frame_id, std::string label, float x, float y);
    Button*                create_button (std::string label, float x, float y, float w, float h);
    Button*                create_button (std::string frame_id, std::string label, float x, float y, float w, float h, const Style& = {});
    Label*                 create_label (std::string frame_id, std::string text, float x, float y);
    inline void            set_color (color_t c) { get_io().font_color = c; }
    void                   frame_end();

    template <class... Args>
    void text (const Vec2& pos, std::format_string<Args...> fmt, Args&&... args)
    {
        IO&         io     = get_io();
        std::string result = std::format (fmt, std::forward<Args> (args)...);
        io.backend->draw_text (pos, result, io.font_color);
        io.font_color = white;
    }

} // namespace AGUI

#endif
