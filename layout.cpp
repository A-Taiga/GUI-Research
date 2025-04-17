#include "layout.h"
#include <algorithm>

namespace
{
    AGUI::Layout::Layout_state state;
}

void AGUI::Layout::Collection::translate (const Point& p)
{
    pos.x += p.x;
    pos.y += p.y;
}
AGUI::Vec2 AGUI::Layout::Collection::size (void) const
{
    return _size;
}

AGUI::Point AGUI::Layout::Collection::position (void) const
{
    return pos;
}

void AGUI::Layout::Collection::insert (std::shared_ptr<Stackable> s)
{
    collection.push_back (s);
}

void AGUI::Layout::Hstack::draw (const Vec2& clip)
{
    float prev_x = 0;
    for (auto& c : collection)
    {
        c->translate ({pos.x + prev_x, pos.y});
        if (c->position().x < clip.x)
            c->draw (clip);
        c->translate ({-pos.x - prev_x, -pos.y});
        prev_x = prev_x + c->size().x + 5;
    }
}

void AGUI::Layout::Hstack::calc_size (void)
{
    for (const auto& i : collection)
    {
        _size.x = std::max (_size.x, i->size().x);
        _size.y = std::max (_size.y, i->size().y);
    }
}

void AGUI::Layout::Vstack::draw (const Vec2& clip)
{
    float prev_y = 0;
    for (auto& c : collection)
    {
        c->translate ({pos.x, pos.y + prev_y});
        if (c->position().y < clip.y)
            c->draw (clip);
        c->translate ({-pos.x, -pos.y - prev_y});
        prev_y = prev_y + c->size().y + 5;
    }
}

void AGUI::Layout::Vstack::calc_size (void)
{
    for (const auto& i : collection)
    {
        _size.x = std::max (_size.x, i->size().x);
        _size.y = std::max (_size.y, i->size().y);
    }
}

AGUI::Layout::Layout_state& AGUI::Layout::get_layout_state()
{
    return state;
}

void AGUI::Layout::push_element (std::shared_ptr<Stackable> s)
{
    state.stk.push (s);
}

void AGUI::Layout::hstack_begin (void)
{
    state.stk.push (std::make_shared<Hstack>());
}

void AGUI::Layout::vstack_begin (void)
{
    state.stk.push (std::make_shared<Vstack>());
}

void combine (AGUI::Layout::Collection* c, std::vector<std::shared_ptr<AGUI::Layout::Stackable>>& v)
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
        Tag  tag = top->tag();
        if (tag == VSTACK || tag == HSTACK)
        {
            Collection* collection = reinterpret_cast<Collection*> (state.stk.top().get());
            if (collection->finished)
            {
                v.push_back (state.stk.top());
                state.stk.pop();
                continue;
            }
            std::ranges::reverse (v);
            combine (collection, v);
            collection->calc_size();
            collection->finished = true;
            break;
        }
        else
        {
            v.push_back (state.stk.top());
            state.stk.pop();
        }
    }
}
/*
 *
 *
 *  [h, b, b, b, v[b,b]
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

/*
namespace
{
    AGUI::Stack_state stack_state;
}

void AGUI::stack_begin()
{
    if (! stack_state.current_collection.empty())
    {
        stack_state.collection.push_back (std::move (stack_state.current_collection));
        stack_state.current_collection = {};
    }
}

void AGUI::hstack_end()
{
    std::shared_ptr<Stackable> new_hstack = std::make_shared<Hstack> (std::move (stack_state.current_collection));
    stack_state.current_collection        = {};
    if (! stack_state.collection.empty())
    {
        stack_state.current_collection = std::move (stack_state.collection.back());
        stack_state.collection.pop_back();
        stack_state.current_collection.push_back (std::move (new_hstack));
    }
    else
    {
        stack_state.current_collection.push_back (std::move (new_hstack));
    }
}

void AGUI::vstack_end()
{
    std::shared_ptr<Stackable> new_vstack = std::make_shared<Vstack> (std::move (stack_state.current_collection));
    stack_state.current_collection        = {};
    if (! stack_state.collection.empty())
    {
        stack_state.current_collection = std::move (stack_state.collection.back());
        stack_state.collection.pop_back();
        stack_state.current_collection.push_back (std::move (new_vstack));
    }
    else
    {
        stack_state.current_collection.push_back (std::move (new_vstack));
    }
}

AGUI::Stack_state& AGUI::get_stack_state()
{
    return stack_state;
}

AGUI::Layout_stack::Layout_stack (std::vector<std::shared_ptr<Stackable>>&& v)
    : collection (std::move (v))
    , _size (0, 0)
{
}
void AGUI::Layout_stack::translate (const Point& p)
{
    pos.x += p.x;
    pos.y += p.y;
}
AGUI::Point AGUI::Layout_stack::position() const { return pos; }
AGUI::Vec2  AGUI::Layout_stack::size() const { return _size; }

AGUI::Hstack::Hstack (std::vector<std::shared_ptr<Stackable>>&& v)
    : Layout_stack (std::move (v))
{
    for (const auto& i : collection)
    {
        _size.x = std::max (_size.x, i->size().x);
        _size.y = std::max (_size.y, i->size().y);
    }
}

void AGUI::Hstack::draw (const Vec2& clip)
{
    float prev_x = 0;
    for (auto& c : collection)
    {
        c->translate ({pos.x + prev_x, pos.y});
        if (c->position().x < clip.x)
            c->draw (clip);
        c->translate ({-pos.x - prev_x, -pos.y});
        prev_x = prev_x + c->size().x + 5;
    }
}

AGUI::Vstack::Vstack (std::vector<std::shared_ptr<Stackable>>&& v)
    : Layout_stack (std::move (v))
{
    for (const auto& i : collection)
    {
        _size.x = std::max (_size.x, i->size().x);
        _size.y = std::max (_size.y, i->size().y);
    }
}

void AGUI::Vstack::draw (const Vec2& clip)
{

    float prev_y = 0;
    for (auto& c : collection)
    {
        c->translate ({pos.x, pos.y + prev_y});
        if (c->position().y < clip.y)
            c->draw (clip);
        c->translate ({-pos.x, -pos.y - prev_y});
        prev_y = prev_y + c->size().y + 5;
    }
}
*/
