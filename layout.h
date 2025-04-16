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
        virtual void  draw()                   = 0;
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

    class Hstack : public Stackable
    {
      public:
        Hstack (std::vector<std::shared_ptr<Stackable>>&& v)
            : collection (std::move (v))
        {
            for (const auto& i : collection)
            {
                _size.x = std::max (_size.x, i->size().x);
                _size.y = std::max (_size.y, i->size().y);
            }
        }

        ~Hstack() {}

        void draw() override
        {
            float prev_x = 0;
            for (auto& c : collection)
            {
                c->translate ({pos.x + prev_x, pos.y});
                c->draw();
                c->translate ({-pos.x - prev_x, -pos.y});
                prev_x = prev_x + c->size().x + 5;
            }
        }

        void translate (const Point& p) override
        {
            pos.x += p.x;
            pos.y += p.y;
        }

        Point position() const override { return pos; }
        Vec2  size() const override { return _size; }

      private:
        std::vector<std::shared_ptr<Stackable>> collection;
        Vec2                                    _size;
        Point                                   pos;
    };

    class Vstack : public Stackable
    {
      public:
        Vstack (std::vector<std::shared_ptr<Stackable>>&& v)
            : collection (std::move (v))
            , _size (0, 0)
        {
            for (const auto& i : collection)
            {
                _size.x = std::max (_size.x, i->size().x);
                _size.y = std::max (_size.y, i->size().y);
            }
        }

        ~Vstack() {}

        void draw() override
        {
            float prev_y = 0;
            for (auto& c : collection)
            {
                c->translate ({pos.x, pos.y + prev_y});
                c->draw();
                c->translate ({-pos.x, -pos.y - prev_y});
                prev_y = prev_y + c->size().y + 5;
            }
        }

        void translate (const Point& p) override
        {
            pos.x += p.x;
            pos.y += p.y;
        }

        Point position() const override { return pos; }
        Vec2  size() const override { return _size; }

      private:
        std::vector<std::shared_ptr<Stackable>> collection;
        Vec2                                    _size;
        Point                                   pos;
    };

} // namespace AGUI

#endif
