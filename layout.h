#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "shapes.h"
#include <memory>
#include <stack>
#include <vector>

#define HSTACK(padding) \
    for (int i = (AGUI::Layout::hstack_begin (padding), 0); i < 1; i = 1, AGUI::Layout::layout_end())

#define VSTACK(padding) \
    for (int i = (AGUI::Layout::vstack_begin (padding), 0); i < 1; i = 1, AGUI::Layout::layout_end())

#define SPACER(w, h) AGUI::Layout::spacer (w, h);

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
        float                                   padding;
        Vec2                                    _size;
        Point                                   pos;
        std::vector<std::shared_ptr<Stackable>> group;
    };
    struct Hstack : public Layout_group
    {
        Hstack (const float _padding) { padding = _padding; }
        ~Hstack() {}
        void draw (const Vec2&) override;
        void calc_size (void) override;
    };

    struct Vstack : public Layout_group
    {
        Vstack (const float _padding) { padding = _padding; }
        ~Vstack() {}
        void draw (const Vec2&) override;
        void calc_size (void) override;
    };

    struct Zstack : public Layout_group
    {
        Zstack() {}
        ~Zstack() {}
        void draw (const Vec2&) override;
        void calc_size (void) override;
    };

    struct Spacer : public Layout_group
    {
        Spacer (float w, float h)
        {
            _size    = {w, h};
            finished = true;
        }
        void draw (const Vec2&) override {}
        void calc_size (void) override {};
        ~Spacer() {}
    };

    struct State
    {
        std::stack<std::shared_ptr<Stackable>> stk;
    };

    void                                      push_element (std::shared_ptr<Stackable>);
    std::optional<std::shared_ptr<Stackable>> top_element (void);
    void                                      hstack_begin (const float padding = 5);
    void                                      vstack_begin (const float padding = 5);
    void                                      zstack_begin (void);
    void                                      spacer (const float w, const float h);
    void                                      layout_end (void);

} // namespace AGUI::Layout

#endif
