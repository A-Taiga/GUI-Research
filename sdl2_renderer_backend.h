#ifndef SDL2_RENDERER_BACKEND_H_
#define SDL2_RENDERER_BACKEND_H_

#include "agui.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct TTF_Font;
union  SDL_Event;


namespace AGUI::SDL2
{

    struct Font_texture
    {
        SDL_Texture* texture;
        int w;
        int h;
    };

    class Backend: public AGUI::Backend
    {
        public:
            Backend (SDL_Window*, SDL_Renderer*);
            ~Backend (void);
            void draw_rect         (const Rect&, const color_t) const override;
            void draw_fill_rect    (const Rect&, const color_t) const override;
            void begin_clip        (const Rect&) const override;
            void end_clip          (void) const override;
            void draw_text         (const Vec2&, std::string_view, color_t = white) const override;
            void calc_text_size    (std::string_view, int *w, int *h) const override;
        
        private:
            [[maybe_unused]] SDL_Window*   window;
            SDL_Renderer* renderer;

            std::array <Font_texture, 256> character_map {};
            TTF_Font* font = nullptr;
    };


    void impl_backend  (SDL_Window*, SDL_Renderer*);
    void impl_event    (const SDL_Event*);
}

#endif