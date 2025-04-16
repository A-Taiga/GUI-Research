#include "widgets.h"
#include "agui.h"

AGUI::Button::Button (std::string _label, float x, float y, float w, float h, const Style& style)
    : label (_label)
    , style_sheet {style}
{
    merge_style (style_sheet, get_style());
    current_bg_color = style_sheet.bg_button.value();
    get_io().backend->calc_text_size (label, &label_w, &label_h);

    float bd_width = style_sheet.bd_width.value();
    box            = Rect (x, y, w, h);
    border         = Rect (x - bd_width, y - bd_width, w + bd_width, h + bd_width);

    int a = 0;
    int b = 0;
    get_io().backend->calc_text_size (label, &a, &b);
}

AGUI::Button::Button (std::string _label, float x, float y, const Style& style)
    : label (_label)
    , style_sheet (style)
{
    merge_style (style_sheet, get_style());

    current_bg_color = style_sheet.bg_button.value();
    get_io().backend->calc_text_size (label, &label_w, &label_h);
    float padding  = style_sheet.padding.value();
    float bd_width = style_sheet.bd_width.value();

    box    = Rect (x, y, label_w + (padding * 2), label_h + (padding * 2));
    border = Rect (x - bd_width, y - bd_width, box.get_width() + bd_width, box.get_height() + bd_width);
}

void AGUI::Button::draw (const Vec2&)
{
    AGUI::IO& io = AGUI::get_io();
    io.backend->draw_fill_rect (box, current_bg_color);
    io.backend->draw_rect (border, AGUI::button_border);

    // AGUI::print ({box.get_center().x - (label_w * .5f), box.get_center().y - (box.get_height() * .5f) + 5}, "{}", label);

    Vec2 p =
        {
            box.get_center().x - (label_w * .5f),
            box.get_center().y - (label_h * .5f)};

    AGUI::text (p, "{}", label);

    // io.backend->draw_rect({
    //     p.x,
    //     p.y,
    //     (float)label_w,
    //     (float)label_h
    // }, red);

    // io.backend->draw_rect({
    //     box.get_center().x - (label_w * .5f),
    //     box.get_center().y - (box.get_height() * .5f) + 5,
    //     (float)label_w,
    //     (float)label_h
    // }, red);

    hover();
    click();
}

void AGUI::Button::translate (const Vec2& p)
{
    box.translate (p);
    border.translate (p);
}

std::string AGUI::Button::ID (void) const
{
    return label;
}

AGUI::Point AGUI::Button::position (void) const
{
    return box.get_min();
}

AGUI::Vec2 AGUI::Button::size (void) const
{
    return border.get_size();
}

const AGUI::Style& AGUI::Button::style (void) const
{
    return style_sheet;
}

void AGUI::Button::move (const Point& p)
{
    box.set_pos (p);
    border.set_pos ({p.x - style_sheet.bd_width.value(), p.y - style_sheet.bd_width.value()});
}

void AGUI::Button::hover (void)
{
    if (box.contains (get_io().mouse_pos))
    {
        current_bg_color = style_sheet.hover_color.value();
        hovering         = true;
    }
    else
    {
        current_bg_color = style_sheet.bg_button.value();
        hovering         = false;
    }
}

void AGUI::Button::click (void)
{
    IO& io = get_io();

    if (! flag && io.mouse_down)
    {
        down_pos = io.mouse_pos;
        flag     = true;
    }
    else if (! io.mouse_down)
    {
        if (flag && box.contains (down_pos) && box.contains (io.mouse_pos))
            register_click = true;
        flag     = false;
        down_pos = {-1, -1};
    }

    if (box.contains (io.mouse_pos) && box.contains (down_pos))
        current_bg_color = style_sheet.click_color.value();
    else if (! box.contains (io.mouse_pos))
        current_bg_color = style_sheet.bg_button.value();

    if (box.contains (io.mouse_pos) && box.contains (down_pos))
        holding = true;
    else if (! box.contains (io.mouse_pos) && ! io.mouse_down)
        holding = false;
}

void AGUI::Button::click_event (std::function<void()> cb)
{
    if (register_click)
    {
        cb();
        register_click = false;
    }
}

void AGUI::Button::hover_event (std::function<void()> cb)
{
    if (hovering)
    {
        cb();
    }
}

void AGUI::Button::hold_event (std::function<void()> cb)
{
    if (holding)
    {
        cb();
    }
}

/* LABEL */
AGUI::Label::Label (std::string _text, float x, float y)
    : text (_text)
    , text_w (0)
    , text_h (0)
{
    merge_style (style_sheet, get_style());

    get_io().backend->calc_text_size (text, &text_w, &text_h);

    float padding  = style_sheet.padding.value();
    float bd_width = style_sheet.bd_width.value();

    box    = Rect (x, y, text_w + (padding * 2), text_h + (padding * 2));
    border = Rect (x - bd_width, y - bd_width, box.get_width() + bd_width, box.get_height() + bd_width);
}

AGUI::Label::Label (std::string _text, float x, float y, float w, float h)
    : text (_text)
{
    merge_style (style_sheet, get_style());
    float bd_width = style_sheet.bd_width.value();
    box            = Rect (x, y, w, h);
    border         = Rect (x - bd_width, y - bd_width, w + bd_width, h + bd_width);
}

void AGUI::Label::draw (const Vec2&)
{
    IO& io = get_io();

    io.backend->draw_text ({position().x, position().y}, text);
}

void AGUI::Label::translate (const Vec2& p)
{
    box.translate (p);
    border.translate (p);
}

std::string AGUI::Label::ID (void) const
{
    return text;
}

AGUI::Point AGUI::Label::position (void) const
{
    return box.get_min();
}

AGUI::Vec2 AGUI::Label::size (void) const
{
    return border.get_size();
}

const AGUI::Style& AGUI::Label::style (void) const
{
    return style_sheet;
}
void AGUI::Label::move (const Point&)
{
}
