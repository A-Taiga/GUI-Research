#ifndef SHAPES_H_
#define SHAPES_H_



namespace AGUI
{
    struct Vec2
    {
        float x,y;
        Vec2 (): x(0.f), y(0.f) {}
        Vec2 (float _x, float _y): x(_x), y(_y){}
    };
    typedef Vec2 Point;

    struct Vec4
    {
        float x,y,w,h;
        Vec4 (): x(0.f), y(0.f), w(0.f), h(0.f) {}
        Vec4 (float _x, float _y, float _w, float _h): x(_x), y(_y), w(_w), h(_h){}
    };



    class Rect
    {
        public:
            Rect (): min(), max() {}
            Rect (float x, float y, float w, float h): min(x,y), max(x+w,y+h){}

            Point get_min     (void) const;
            Point get_max     (void) const;
            Point get_center  (void) const;
            Vec2  get_size    (void) const;
            float get_width   (void) const;
            float get_height  (void) const;
            bool  contains    (const Point&) const;
            bool  contains    (const Rect&) const;
            void  translate   (const Vec2&);
            void  translate_x (float);
            void  translate_y (float);
            void  transform   (const Vec2&);
            void  transform_w (float);
            void  transform_h (float);
            void  set_size    (const Vec2& v);
            void  set_size_w  (float);
            void  set_size_h  (float);
            void  set_pos     (const Point&);
            Vec4  to_Vec4     (void) const;



        private:
            Point min;
            Point max;
    };
}

#endif