#include "layout.h"
#include <algorithm>

namespace
{
    AGUI::Layout::State state;
}

void AGUI::Layout::Layout_group::translate (const Point& p)
{
    pos.x += p.x;
    pos.y += p.y;
}
AGUI::Vec2 AGUI::Layout::Layout_group::size (void) const
{
    return _size;
}

AGUI::Point AGUI::Layout::Layout_group::position (void) const
{
    return pos;
}

void AGUI::Layout::Layout_group::insert (std::shared_ptr<Stackable> s)
{
    group.push_back (s);
}

void AGUI::Layout::Hstack::draw (const Vec2& clip)
{
    float prev_x = 0;
    for (auto& c : group)
    {
        c->translate ({pos.x + prev_x, pos.y});
        if (c->position().x < clip.x)
            c->draw (clip);
        c->translate ({-pos.x - prev_x, -pos.y});
        prev_x = prev_x + c->size().x + padding;
    }
}

void AGUI::Layout::Hstack::calc_size (void)
{
    for (const auto& i : group)
    {
        _size.x = std::max (_size.x, i->size().x);
        _size.y = std::max (_size.y, i->size().y);
    }
}

void AGUI::Layout::Vstack::draw (const Vec2& clip)
{
    float prev_y = 0;
    for (auto& c : group)
    {
        c->translate ({pos.x, pos.y + prev_y});
        if (c->position().y < clip.y)
            c->draw (clip);
        c->translate ({-pos.x, -pos.y - prev_y});
        prev_y = prev_y + c->size().y + padding;
    }
}

void AGUI::Layout::Vstack::calc_size (void)
{
    for (const auto& i : group)
    {
        _size.x = std::max (_size.x, i->size().x);
        _size.y = std::max (_size.y, i->size().y);
    }
}

void AGUI::Layout::Zstack::draw (const Vec2& clip)
{
    for (auto& c : group)
    {
        c->translate ({pos.x, pos.y});
        if (c->position().x < clip.x)
            c->draw (clip);
        c->translate ({-pos.x, -pos.y});
    }
}

void AGUI::Layout::Zstack::calc_size (void)
{
    for (const auto& i : group)
    {
        _size.x = std::max (_size.x, i->size().x);
        _size.y = std::max (_size.y, i->size().y);
    }
}

void AGUI::Layout::push_element (std::shared_ptr<Stackable> s)
{
    state.stk.push (s);
}

std::optional<std::shared_ptr<AGUI::Layout::Stackable>> AGUI::Layout::top_element (void)
{
    if (! state.stk.empty())
    {
        return state.stk.top();
    }
    return {};
}

void AGUI::Layout::hstack_begin (const float padding)
{
    state.stk.push (std::make_shared<Hstack> (padding));
}

void AGUI::Layout::vstack_begin (const float padding)
{
    state.stk.push (std::make_shared<Vstack> (padding));
}
void AGUI::Layout::zstack_begin (void)
{
    state.stk.push (std::make_shared<Zstack>());
}

void AGUI::Layout::spacer (float w, float h)
{
    state.stk.push (std::make_shared<Spacer> (w, h));
}
void combine (AGUI::Layout::Layout_group* c, std::vector<std::shared_ptr<AGUI::Layout::Stackable>>& v)
{
    for (auto& i : v)
    {
        c->insert (i);
    }
}

void AGUI::Layout::layout_end (void)
{
    std::vector<std::shared_ptr<Stackable>> v;
    while (! state.stk.empty())
    {
        auto top = state.stk.top();
        if (state.stk.top()->tag() == LAYOUT_GROUP)
        {
            Layout_group* group = reinterpret_cast<Layout_group*> (state.stk.top().get());
            if (group->finished)
            {
                v.push_back (state.stk.top());
                state.stk.pop();
                continue;
            }
            std::ranges::reverse (v);
            combine (group, v);
            group->calc_size();
            group->finished = true;
            break;
        }
        else
        {
            v.push_back (state.stk.top());
            state.stk.pop();
        }
    }
}
