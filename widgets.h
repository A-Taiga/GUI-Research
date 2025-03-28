#ifndef WIDGETS_H_
#define WIDGETS_H_

#include <cstdint>
#include <functional>
#include <string>
#include "shapes.h"
#include "agui.h"

namespace AGUI
{
    using color_t = uint32_t;
    struct Widget
    {
        virtual ~Widget (void) {}
        virtual void          draw      (void)         = 0;
        virtual void          translate (const Vec2&)  = 0;
        virtual std::string   ID        (void) const   = 0;
        virtual Point         position  (void) const   = 0;
        virtual Vec2          size      (void) const   = 0;
        virtual const Style&  style     (void) const   = 0;
        virtual void          move      (const Point&) = 0;
    };

    class Button : public Widget
    {
        public:
            Button (std::string _label, float x, float y, float w, float h, const Style& = {});
            Button (std::string _label, float x, float y, const Style& = {});
            virtual ~Button () {}   
            virtual void        draw         (void) override;
            virtual void        translate    (const Vec2&) override;
            virtual std::string ID           (void) const final override;
            virtual Point       position     (void) const override;
            virtual Vec2        size         (void) const override;
            const   Style&      style        (void) const final override;
            virtual void        move         (const Point&) override;
            virtual void        hover        (void);
            virtual void        click        (void);
            virtual void        click_event  (std::function <void()>);
            virtual void        hover_event  (std::function <void()>);
            virtual void        hold_event   (std::function <void()>);

        private:
            std::string label;
            Style       style_sheet;
            Rect        box;
            Rect        border;
            color_t     current_bg_color;
            int         label_w        = 0;
            int         label_h        = 0;
            bool        flag           = false;
            bool        register_click = false;
            Point       down_pos       = {-1,-1};
            bool        hovering       = false;
            bool        holding        = false;

            std::function <void()> event_cb;
    };

    class Label : public Widget
    {
        public:
            Label (std::string text, float x, float y);
            virtual ~Label () {}
            virtual void          draw      (void) override;
            virtual void          translate (const Vec2&) override;
            std::string           ID        (void) const final;
            virtual Point         position  (void) const override;
            virtual Vec2          size      (void) const override;
            const Style&          style     (void) const final;
            virtual void          move      (const Point&) override;
            
        private:
            std::string text;
            Style style_sheet;
            Rect box;
            Rect border;
            int text_w;
            int text_h;
    };
}

#endif