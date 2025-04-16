#include "layout.h"

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
