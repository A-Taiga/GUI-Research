#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "shapes.h"
#include <memory>
#include <stack>
#include <vector>

#define HSTACK() \
    for (int i = (AGUI::Layout::hstack_begin(), 0); i < 1; i = 1, AGUI::Layout::layout_end())

#define VSTACK() \
    for (int i = (AGUI::Layout::vstack_begin(), 0); i < 1; i = 1, AGUI::Layout::layout_end())

namespace AGUI::Layout
{

    enum Tag
    {
        OTHER,
        LAYOUT_GROUP,
    };

    struct Stackable
    {
        virtual ~Stackable() {};
        virtual void  draw (const Vec2&)       = 0;
        virtual void  translate (const Point&) = 0;
        virtual Vec2  size (void) const        = 0;
        virtual Point position (void) const    = 0;
        virtual Tag   tag (void) { return OTHER; }
    };

    struct Layout_group : public Stackable
    {
        Layout_group() {}
        ~Layout_group() {}
        virtual void draw (const Vec2&) override = 0;
        void         translate (const Point&) override final;
        Vec2         size (void) const override final;
        Point        position (void) const override final;
        void         insert (std::shared_ptr<Stackable>);
        virtual void calc_size (void) = 0;
        virtual Tag  tag (void) override { return LAYOUT_GROUP; }
        bool         finished = false;

      protected:
        Vec2                                    _size;
        Point                                   pos;
        std::vector<std::shared_ptr<Stackable>> group;
    };

    struct Hstack : public Layout_group
    {
        Hstack() {}
        ~Hstack() {}
        void draw (const Vec2&) override;
        void calc_size (void) override;
    };

    struct Vstack : public Layout_group
    {
        Vstack() {}
        ~Vstack() {}
        void draw (const Vec2&) override;
        void calc_size (void) override;
    };

    struct Layout_state
    {
        std::stack<std::shared_ptr<Stackable>> stk;
    };

    Layout_state& get_layout_state();
    void          push_element (std::shared_ptr<Stackable>);
    void          hstack_begin (void);
    void          vstack_begin (void);
    void          layout_end (void);

} // namespace AGUI::Layout

#endif
