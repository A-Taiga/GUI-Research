#include "agui.h"
#include "layout.h"
#include "shapes.h"
#include "widgets.h"
#include <iterator>
#include <limits>
#include <memory>

namespace
{
    AGUI::Context ctx {};
} // namespace

/* default style */
AGUI::Style::Style()
{
    bg_frame    = frame_bg;
    bd_color    = button_border;
    hover_color = button_hover;
    click_color = button_click;
    bg_button   = button_background;
    bd_width    = 1;
    padding     = 5;
};

AGUI::IO& AGUI::get_io (void) { return ctx.io; }

const AGUI::Style& AGUI::get_style (void) { return ctx.style; }

std::shared_ptr<AGUI::Frame> AGUI::create_frame (std::string name, float x, float y, float w, float h, uint32_t options, const Style& style)
{
    AGUI_ASSERT (! ctx.frames_map.contains (name) && "Frame id already in use");
    ctx.frames.push_back (std::make_shared<Frame> (name, x, y, w, h, options, style));
    ctx.frames_map[name] = std::prev (ctx.frames.end());
    ctx.frame_order.push_back (ctx.frames.back());
    ctx.order_map[name] = std::prev (ctx.frame_order.end());
    return ctx.frames.back();
}

AGUI::Button* AGUI::create_button (std::string label, float x, float y)
{
    auto current_frame = ctx.frames.back();
    auto button        = std::make_shared<Button> (label, x, y);
    get_stack_state().current_collection.push_back (button);
    return button.get();
}

AGUI::Button* AGUI::create_button (std::string label, float x, float y, float w, float h)
{
    auto current_frame = ctx.frames.back();
    auto button        = std::make_shared<Button> (label, x, y, w, h);
    get_stack_state().current_collection.push_back (button);
    return button.get();
}

AGUI::Label* AGUI::create_label (std::string text, float x, float y)
{
    auto current_frame = ctx.frames.back();
    auto label         = std::make_shared<Label> (text, x, y);
    get_stack_state().current_collection.push_back (label);
    return label.get();
}

AGUI::Label* AGUI::create_label (std::string text, float x, float y, float w, float h)
{
    auto current_frame = ctx.frames.back();
    auto label         = std::make_shared<Label> (text, x, y, w, h);
    get_stack_state().current_collection.push_back (label);
    return label.get();
}

void AGUI::update (void)
{
    for (auto it = ctx.frame_order.begin(); it != ctx.frame_order.end(); ++it)
    {
        auto frame = it->lock();
        if (frame->move())
        {
            if (ctx.focused_frame.expired())
                ctx.focused_frame = *it;
            else if (ctx.focused_frame.lock()->ID() == frame->ID())
                break;

            ctx.frame_order.erase (it);
            ctx.frame_order.push_front (frame);
            ctx.order_map[frame->ID()] = ctx.frame_order.begin();
            ctx.focused_frame          = frame;
            break;
        }
    }

    std::for_each (ctx.frame_order.rbegin(), ctx.frame_order.rend(),
                   [] (std::weak_ptr<Frame>& frame)
                   { frame.lock()->draw(); });
}

AGUI::Frame::Frame (std::string _name, float x, float y, float w, float h, uint32_t _options, const Style& _style)
    : options {_options}
    , name {_name}
    , position {x, y}
    , size {w, h}
    , style {_style}
{
    (void) options;
    merge_style (style, ctx.style);

    float bd_width = style.bd_width.value();
    float padding  = style.padding.value();
    position.x     = x + style.bd_width.value();
    position.y     = y + style.bd_width.value();

    frame_bar = Rect (0, 0, w + style.padding.value(), 33 + (padding * 2));
    content   = Rect (0, frame_bar.get_height(), frame_bar.get_width(), h + frame_bar.get_height());
    border    = Rect (-bd_width, -bd_width, frame_bar.get_width() + (bd_width * 2), frame_bar.get_height() + content.get_height() + (bd_width * 2));

    resize_box = Rect (frame_bar.get_width() - 15, border.get_height() - 15 - bd_width, 15, 15);

    int tx = 0;
    int ty = 0;
    ctx.io.backend->calc_text_size ("-", &tx, &ty);

    Style s;
    s.padding = -5;

    minimize_button = std::make_unique<Button> ("-", w - 68, 0, 33, 33);
    close_button    = std::make_unique<Button> ("X", w - 33, 0, 33, 33);

    get_io().backend->calc_text_size ("0", &tx, &ty);
}

void AGUI::Frame::draw (void)
{
    IO&   io      = ctx.io;
    float padding = style.padding.value();

    resize();

    /* frame bar  */
    frame_bar.translate (position);
    io.backend->draw_fill_rect (frame_bar, AGUI::frame_bar);
    frame_bar.translate ({-position.x, -position.y});

    /* border */
    border.translate (position);
    io.backend->draw_rect (border, style.bd_color.value());
    border.translate ({-position.x, -position.y});

    text ({position.x + padding, position.y + padding}, "{}", name);

    /* close button */
    close_button->translate ({position.x, position.y + padding});
    close_button->draw();
    close_button->translate ({-position.x, -position.y - padding});

    /* minimize button */
    minimize_button->translate ({position.x, position.y + padding});
    minimize_button->draw();
    minimize_button->translate ({-position.x, -position.y - padding});
    minimize_button->click_event (
        [&]()
        {
            if (minimized)
            {
                border.set_size_h (content.get_height() + frame_bar.get_height() + style.bd_width.value());
                minimized = false;
            }
            else
            {
                border.set_size_h (frame_bar.get_height());
                minimized = true;
            }
        });

    if (minimized)
        return;

    /* content */
    content.translate (position);
    io.backend->draw_fill_rect (content, style.bg_frame.value());
    content.translate ({-position.x, -position.y});

    content.translate (position);
    io.backend->begin_clip (content);
    content.translate ({-position.x, -position.y});

    content.translate (position);
    for (auto& c : collection)
    {
        const float fh = frame_bar.get_height();
        c->translate ({position.x + padding, position.y + fh + padding});
        c->draw();
        c->translate ({-position.x - padding, -position.y - fh - padding});
    }
    content.translate ({-position.x, -position.y});

    io.backend->end_clip();

    /* resize */
    resize_box.translate (position);
    io.backend->draw_fill_rect (resize_box, style.bd_color.value());
    resize_box.translate ({-position.x, -position.y});
}

bool AGUI::Frame::move (void)
{
    bool no_move = false;
    minimize_button->hold_event ([&no_move]()
                                 { no_move = true; });
    close_button->hold_event ([&no_move]()
                              { no_move = true; });

    if (no_move)
        return true;

    IO& io = get_io();

    if (io.mouse_down && ! mouse_was_down)
    {
        mouse_was_down = true;
        mouse_down_pos = io.mouse_pos;
    }
    else if (! io.mouse_down)
    {
        mouse_was_down     = false;
        frame_bar_selected = false;
    }

    frame_bar.translate ({position.x, position.y});
    bool check = frame_bar.contains (mouse_down_pos);
    frame_bar.translate ({-position.x, -position.y});

    if (! check)
        return false;

    mouse_down_pos = io.mouse_pos;

    if (! frame_bar_selected && io.mouse_down)
    {
        frame_bar.translate ({position.x, position.y});
        bool check = frame_bar.contains (mouse_down_pos);
        frame_bar.translate ({-position.x, -position.y});

        if (check)
        {
            frame_bar_selected = true;
            move_offset.x      = io.mouse_pos.x - position.x;
            move_offset.y      = io.mouse_pos.y - position.y;
        }
    }
    else if (! io.mouse_down)
        frame_bar_selected = false;

    if (frame_bar_selected)
    {
        position.x = io.mouse_pos.x - move_offset.x;
        position.y = io.mouse_pos.y - move_offset.y;
        return true;
    }
    return false;
}

void AGUI::Frame::resize (void)
{

    IO& io = get_io();

    if (minimized)
        return;

    if (! resizing && io.mouse_down)
    {
        resize_box.translate ({position.x, position.y});
        bool check = resize_box.contains (io.mouse_pos);
        resize_box.translate ({-position.x, -position.y});

        if (check)
        {
            resizing = true;
            resize_offset.x =
                io.mouse_pos.x - border.get_width() + style.bd_width.value();
            resize_offset.y =
                io.mouse_pos.y - border.get_height() + style.bd_width.value();
        }
    }
    else if (resizing && ! io.mouse_down)
        resizing = false;

    if (resizing)
    {
        int x = 0;
        int y = 0;
        io.backend->calc_text_size (name, &x, &y);

        float bd_width = style.bd_width.value();
        float padding  = style.padding.value();
        float min      = (float) x + (padding * 2) + padding + (33 * 2) + padding;
        float clamp_x  = std::clamp (io.mouse_pos.x - resize_offset.x, min, std::numeric_limits<float>::infinity());
        float clamp_y  = std::clamp (io.mouse_pos.y - resize_offset.y, frame_bar.get_height() + resize_box.get_height(), std::numeric_limits<float>::infinity());

        frame_bar.set_size_w (clamp_x - bd_width);
        content.set_size ({clamp_x - bd_width, clamp_y - bd_width});
        border.set_size ({clamp_x, clamp_y});
        resize_box.set_pos ({frame_bar.get_width() - resize_box.get_width(), clamp_y - resize_box.get_height()});

        minimize_button->move ({frame_bar.get_width() - 68 - padding, 0});
        close_button->move ({frame_bar.get_width() - 33 - padding, 0});
    }
}

std::string AGUI::Frame::ID (void) const { return name; }

void AGUI::Frame::add_element (std::shared_ptr<Stackable> s)
{
    collection.push_back (std::move (s));
}

void AGUI::frame_end()
{
    for (auto& s : get_stack_state().current_collection)
    {
        ctx.frames.back()->add_element (std::move (s));
    }
    get_stack_state().collection         = {};
    get_stack_state().current_collection = {};
}

/*
void AGUI::vstack_end()
{

    std::shared_ptr<Stackable> new_vstack = std::make_shared<Vstack> (std::move (stack_state.current_collection));
    stack_state.current_collection        = std::move (stack_state.collection);
    stack_state.current_collection.push_back (std::move (new_vstack));
    stack_state.collection = {};
}
*/
/*
 * HSTACK()
 * {
 *    button
 *    button
 *
 *    VSTACK()
 *    {
 *
 *    }
 * }
 */
