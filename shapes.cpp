#include "shapes.h"



AGUI::Point AGUI::Rect::get_min (void) const 
{
    return min;
}

AGUI::Point AGUI::Rect::get_max (void) const 
{
    return max;
}

AGUI::Point AGUI::Rect::get_center (void) const 
{
    return {(min.x + max.x) * .5f, (min.y + max.y) * .5f};
}

AGUI::Vec2  AGUI::Rect::get_size   (void) const 
{
    return {max.x - min.x, max.y - min.y};
}

float AGUI::Rect::get_width  (void) const 
{
    return max.x - min.x;
}

float AGUI::Rect::get_height (void) const 
{
    return max.y - min.y;
}

bool AGUI::Rect::contains (const Point& p) const
{
    return p.x >= min.x &&
           p.x <= max.x && 
           p.y >= min.y &&
           p.y <= max.y;
}

bool AGUI::Rect::contains (const Rect& r) const
{
    const Point r_min = r.get_min();
    const Point r_max = r.get_max();

    return r_min.x >= min.x && 
           r_max.x <= max.x && 
           r_min.y >= min.y && 
           r_max.y <= max.y;
}

void AGUI::Rect::translate  (const Vec2& v)
{
    min.x += v.x;
    min.y += v.y;
    max.x += v.x;
    max.y += v.y;
}

void AGUI::Rect::translate_x (float x)
{
    min.x += x;
    max.x += x;
}

void AGUI::Rect::translate_y (float y)
{
    min.y += y;
    max.y += y;
}


void AGUI::Rect::transform (const Vec2& s)
{
    max.x += s.x;
    max.y += s.y;
}

void AGUI::Rect::transform_w (float w)
{
    max.x += w;
}

void AGUI::Rect::transform_h (float h)
{
    max.y += h;
}

void AGUI::Rect::set_size (const Vec2& v)
{
    max.x = v.x;
    max.y = v.y;
}

void AGUI::Rect::set_size_w  (float w)
{
    max.x = w;
}

void AGUI::Rect::set_size_h  (float h)
{
    max.y = h;
}

void AGUI::Rect::set_pos (const Point& p)
{
    const auto w = get_width();
    const auto h = get_height();
    min = p;
    max.x = p.x + w;
    max.y = p.y + h;
}

AGUI::Vec4 AGUI::Rect::to_Vec4 (void) const
{
    return {min.x, min.y, max.x - min.x, max.y - min.y};
}

