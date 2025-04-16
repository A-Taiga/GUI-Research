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
