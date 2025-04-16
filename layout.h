#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "shapes.h"
#include <memory>
#include <vector>

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

#endif
