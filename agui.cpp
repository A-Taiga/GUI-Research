#include "agui.h"
#include <iterator>
#include <limits>
#include <memory>
#include "widgets.h"

namespace
{
    AGUI::Context ctx {};
}

/* default style */
AGUI::Style::Style ()
{
    bg_frame    = black;
    bd_color    = button_border;
    hover_color = button_hover;
    click_color = button_click;
    bg_button   = button_background;
    bd_width    = 1;
    padding     = 5;
};

AGUI::IO& AGUI::get_io (void)
{
    return ctx.io;
}

const AGUI::Style& AGUI::get_style (void)
{
    return ctx.style;
}

void AGUI::create_frame (std::string name, float x, float y, float w, float h, const Style& style)
{
    AGUI_ASSERT(!ctx.frames_map.contains(name) && "Frame id already in use");
    ctx.frames.push_back (std::make_shared <Frame> (name, x, y, w, h, style));
    ctx.frames_map [name] = std::prev (ctx.frames.end());
    ctx.frame_order.push_back(ctx.frames.back());
    ctx.order_map[name] = std::prev (ctx.frame_order.end());
}

void AGUI::create_button (std::string frame_id, std::string label, float x, float y)
{
    AGUI_ASSERT (ctx.frames_map.contains(frame_id) && "Frame does not exist");

    AGUI_ASSERT (!(*ctx.frames_map[frame_id])->has_widget_id(label + "##Button") && "Button already has that label");

    (*ctx.frames_map[frame_id])->add_widget(std::make_shared<Button> (label, x, y));
}

void AGUI::create_label  (std::string frame_id, std::string text, float x, float y)
{
    AGUI_ASSERT(ctx.frames_map.contains(frame_id) && "Frame does not exist");

    (*ctx.frames_map[frame_id])->add_widget(std::make_shared<Label> (text, x, y));
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
            ctx.focused_frame = frame;
            break;
        }
    }

    std::for_each (ctx.frame_order.rbegin(), ctx.frame_order.rend(), [](std::weak_ptr <Frame>& frame)
    {
        frame.lock()->draw();
    });
}


AGUI::Frame::Frame (std::string _name, float x, float y, float w, float h, const Style& _style)
: name {_name}
, position {x,y}
, size {w,h}
, style {_style}
{
    merge_style(style, ctx.style);
    float bd_width = style.bd_width.value();
    float padding  = style.padding.value();
    frame_bar  = Rect (0, 0, w + style.padding.value(), 35 + padding);
    content    = Rect (0, 40, w + style.padding.value(), h - 40);
    border     = Rect (-bd_width, -bd_width, w + bd_width + style.padding.value(), h + bd_width);
    resize_box = Rect (w - 15 + padding, h - 15, 15, 15);


    int tx = 0;
    int ty = 0;
    ctx.io.backend->calc_text_size("-", &tx, &ty);
    Style s;
    s.padding = -5;

    minimize_button = std::make_unique <Button> ("-", w - 68, 0, 33, 33);
    close_button = std::make_unique <Button> ("X", w - 33, 0, 33, 33);
}

void AGUI::Frame::draw (void)
{
    IO& io = ctx.io;
    float padding = style.padding.value();

    resize();

    frame_bar.translate({position.x, position.y});
    io.backend->draw_fill_rect(frame_bar, AGUI::frame_bar);
    frame_bar.translate({-position.x, -position.y});

    content.translate({position.x, position.y});
    io.backend->draw_fill_rect(content, style.bg_frame.value());
    content.translate({-position.x, -position.y});

    resize_box.translate({position.x, position.y});
    io.backend->draw_fill_rect(resize_box, style.bd_color.value());
    resize_box.translate({-position.x, -position.y});

    border.translate({position.x, position.y});
    io.backend->draw_rect(border, style.bd_color.value());
    border.translate({-position.x, -position.y});
    
    text ({position.x + padding, position.y + padding}, "{}", name);

    minimize_button->translate({position.x, position.y + padding});
    minimize_button->draw();
    minimize_button->translate({-position.x, -position.y - padding});

    close_button->translate({position.x, position.y + padding});
    close_button->draw();
    close_button->translate({-position.x, -position.y - padding});

    content.translate({position.x, position.y});
    io.backend->begin_clip (content);
    content.translate({-position.x, -position.y});


    content.translate({position.x, position.y});
    for (auto& widget : widgets)
    {
        const float fh = frame_bar.get_height();
        widget->translate({position.x + padding, position.y + fh + padding});
        if (widget->position().y + widget->size().y < content.get_max().y + 50)
            widget->draw();
        widget->translate({-position.x - padding, -position.y - fh - padding});
    }
    content.translate({-position.x, -position.y});

    io.backend->end_clip();

}

bool AGUI::Frame::move (void)
{
    IO& io = get_io();

    if (io.mouse_down && !mouse_was_down)
    {
        mouse_was_down = true;
        mouse_down_pos = io.mouse_pos;
    }
    else if (!io.mouse_down)
    {
        mouse_was_down = false;
        frame_bar_selected = false;
    }

    {
        frame_bar.translate ({position.x, position.y});
        bool check = frame_bar.contains(mouse_down_pos);
        frame_bar.translate ({-position.x, -position.y});

        if (!check)
            return false;
    }

    mouse_down_pos = io.mouse_pos;

    if (!frame_bar_selected && io.mouse_down)
    {
        frame_bar.translate ({position.x, position.y});
        bool check = frame_bar.contains(mouse_down_pos);
        frame_bar.translate ({-position.x, -position.y});

        if (check)
        {
            frame_bar_selected = true;
            move_offset.x = io.mouse_pos.x - position.x;
            move_offset.y = io.mouse_pos.y - position.y;
        }
    }
    else if (!io.mouse_down)
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

    if (!resizing && io.mouse_down)
    {
        resize_box.translate({position.x, position.y});
        bool check = resize_box.contains(io.mouse_pos);
        resize_box.translate({-position.x, -position.y});

        if (check)
        {
            resizing = true;
            resize_offset.x  = io.mouse_pos.x - border.get_size().x + style.bd_width.value();
            resize_offset.y  = io.mouse_pos.y - border.get_size().y + style.bd_width.value();
        }
    }
    else if (resizing && !io.mouse_down)
        resizing = false;

    if (resizing)
    {
        int x = 0;
        int y = 0;
        io.backend->calc_text_size(name, &x, &y);
        float min = (float)x + (style.padding.value() * 2) + style.padding.value() + (33 * 2) + style.padding.value();
        float clamp_x = std::clamp (io.mouse_pos.x - resize_offset.x, min, std::numeric_limits<float>::infinity());
        float clamp_y = std::clamp (io.mouse_pos.y - resize_offset.y, frame_bar.get_height() + resize_box.get_height(), std::numeric_limits<float>::infinity());
        frame_bar.set_size_w (clamp_x);
        content.set_size     ({clamp_x, clamp_y});
        border.set_size      ({clamp_x, clamp_y});
        resize_box.set_pos   
        (
            {
                frame_bar.get_width() - resize_box.get_width(), 
                clamp_y - resize_box.get_height()
            }
        );
        minimize_button->move({frame_bar.get_width() - 68 - style.padding.value(), 0});
        close_button->move({frame_bar.get_width() - 33 - style.padding.value(), 0});
    }
}

std::string AGUI::Frame::ID (void) const
{
    return name;
}

void AGUI::Frame::add_widget (const std::shared_ptr <Widget> widget)
{
    widgets.push_back (widget);
    widget_map[widget->ID() + "##Button"] = std::prev(widgets.end());
}

bool AGUI::Frame::has_widget_id (const std::string& id) const
{
    return widget_map.contains (id);
}