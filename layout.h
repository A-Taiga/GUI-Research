#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "shapes.h"
#include <memory>
#include <stack>
#include <vector>
/*
#define HSTACK() \
    for (int i = (AGUI::stack_begin(), 0); i < 1; i = 1, AGUI::hstack_end())

#define VSTACK() \
    for (int i = (AGUI::stack_begin(), 0); i < 1; i = 1, AGUI::vstack_end())

namespace AGUI
{
    struct Stackable
    {
        virtual ~Stackable() {};
        virtual void  draw (const Vec2&)       = 0;
        virtual void  translate (const Point&) = 0;
        virtual Vec2  size() const             = 0;
        virtual Point position (void) const    = 0;
    };
    struct Stack_state
    {
        std::vector<std::vector<std::shared_ptr<Stackable>>> collection;
        std::vector<std::shared_ptr<Stackable>>              current_collection;
    };

    void         stack_begin();
    void         hstack_end();
    void         vstack_end();
    Stack_state& get_stack_state();

    class Layout_stack : public Stackable
    {
      public:
        Layout_stack (std::vector<std::shared_ptr<Stackable>>&& v);
        void  translate (const Point& p) override final;
        Point position() const override final;
        Vec2  size (void) const override final;
        void  draw (const Vec2&) override = 0;

      protected:
        std::vector<std::shared_ptr<Stackable>> collection;
        Vec2                                    _size;
        Point                                   pos;
    };

    class Hstack : public Layout_stack
    {
      public:
        Hstack (std::vector<std::shared_ptr<Stackable>>&&);
        ~Hstack() {}
        void draw (const Vec2&) override;
    };

    class Vstack : public Layout_stack
    {
      public:
        Vstack (std::vector<std::shared_ptr<Stackable>>&& v);
        ~Vstack() {}
        void draw (const Vec2&) override;
    };

} // namespace AGUI
*/

#define HSTACK() \
    for (int i = (AGUI::Layout::hstack_begin(), 0); i < 1; i = 1, AGUI::Layout::layout_end())

#define VSTACK() \
    for (int i = (AGUI::Layout::vstack_begin(), 0); i < 1; i = 1, AGUI::Layout::layout_end())

namespace AGUI::Layout
{

    enum Tag
    {
        OTHER,
        HSTACK,
        VSTACK,
        COLLECTION,
    };

    struct Stackable
    {
        virtual ~Stackable() {};
        virtual void  draw (const Vec2&)       = 0;
        virtual void  translate (const Point&) = 0;
        virtual Vec2  size (void) const        = 0;
        virtual Point position (void) const    = 0;
        virtual Tag   tag (void)               = 0;
    };

    struct Collection : public Stackable
    {
        Collection() {}
        ~Collection() {}
        virtual void draw (const Vec2&) override = 0;
        void         translate (const Point&) override final;
        Vec2         size (void) const override final;
        Point        position (void) const override final;
        void         insert (std::shared_ptr<Stackable>);
        virtual void calc_size (void) = 0;
        virtual Tag  tag (void) override { return COLLECTION; }
        bool         finished = false;

      protected:
        Vec2                                    _size;
        Point                                   pos;
        std::vector<std::shared_ptr<Stackable>> collection;
    };

    struct Hstack : public Collection
    {
        Hstack() {}
        ~Hstack() {}
        void draw (const Vec2&) override;
        void calc_size (void) override;
        Tag  tag (void) override { return HSTACK; }
    };

    struct Vstack : public Collection
    {
        Vstack() {}
        ~Vstack() {}
        void draw (const Vec2&) override;
        void calc_size (void) override;
        Tag  tag (void) override { return VSTACK; }
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
