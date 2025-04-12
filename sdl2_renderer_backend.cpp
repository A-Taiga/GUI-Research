#include "sdl2_renderer_backend.h"
#include "agui.h"
#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <memory>

#include "shapes.h"

void AGUI::SDL2::impl_backend (SDL_Window* window, SDL_Renderer* renderer)
{
  IO& io = get_io();
  AGUI_ASSERT (io.backend == nullptr && "Backend already initilized");
  io.backend = std::make_shared<AGUI::SDL2::Backend> (window, renderer);
}

void AGUI::SDL2::impl_event (const SDL_Event* event)
{
  IO& io = get_io();
  AGUI_ASSERT (io.backend != nullptr && "Backend is NULL")

  if (event->type == SDL_MOUSEMOTION)
  {
    io.mouse_pos = {(float) event->motion.x, (float) event->motion.y};
  }
  else if (event->type == SDL_MOUSEBUTTONDOWN)
    io.mouse_down = true;
  else if (event->type == SDL_MOUSEBUTTONUP)
    io.mouse_down = false;
  else if (event->type == SDL_MOUSEMOTION)
    ;
}

AGUI::SDL2::Backend::Backend (SDL_Window* _window, SDL_Renderer* _renderer)
    : window (_window)
    , renderer (_renderer)
{
  TTF_Init();
  IO& io = get_io();

  font = TTF_OpenFont (io.font_path, io.font_size);
  if (font == NULL)
  {
    fprintf (stderr, "error: font file error\n");
    exit (EXIT_FAILURE);
  }

  char i = 0;
  for (auto& character : character_map)
  {
    TTF_SizeText (font, &i, &character.w, &character.h);
    SDL_Surface* font_surface = TTF_RenderText_Blended (font, &i, {255, 255, 255, 255});
    character.texture         = SDL_CreateTextureFromSurface (renderer, font_surface);
    SDL_FreeSurface (font_surface);
    ++i;
  }
}

AGUI::SDL2::Backend::~Backend (void)
{
  TTF_CloseFont (font);
}

void AGUI::SDL2::Backend::draw_rect (const Rect& r, const color_t c) const
{
  const Vec2 min = r.get_min();
  SDL_FRect  rect =
      {
          min.x,
          min.y,
          r.get_width(),
          r.get_height()};

  SDL_SetRenderDrawColor (renderer, TO_RGBA (c));
  SDL_RenderDrawRectF (renderer, &rect);
  SDL_SetRenderDrawColor (renderer, TO_RGBA (0x000000FF));
}

void AGUI::SDL2::Backend::draw_fill_rect (const Rect& r, const color_t c) const
{
  const Vec2 min = r.get_min();
  SDL_FRect  rect =
      {
          min.x,
          min.y,
          r.get_width(),
          r.get_height()};

  SDL_SetRenderDrawColor (renderer, TO_RGBA (c));
  SDL_RenderFillRectF (renderer, &rect);
  SDL_SetRenderDrawColor (renderer, TO_RGBA (0x000000FF));
}

void AGUI::SDL2::Backend::begin_clip (const Rect& r) const
{
  SDL_Rect rect {
      (int) r.get_min().x,
      (int) r.get_min().y,
      (int) r.get_width(),
      (int) r.get_height()};

  SDL_RenderSetClipRect (renderer, &rect);
}

void AGUI::SDL2::Backend::end_clip() const
{
  SDL_RenderSetClipRect (renderer, nullptr);
}

void AGUI::SDL2::Backend::draw_text (const Vec2& pos, std::string_view str, color_t color) const
{
  float currecnt_x = pos.x;
  for (const auto& c : str)
  {
    const auto& character = character_map[c];
    SDL_FRect   rect      = {currecnt_x, pos.y, (float) character.w, (float) character.h};
    SDL_SetTextureColorMod (character.texture, TO_RGB (color));
    SDL_RenderCopyF (renderer, character.texture, nullptr, &rect);
    currecnt_x += character.w;
  }
}

void AGUI::SDL2::Backend::calc_text_size (std::string_view str, int* w, int* h) const
{
  for (char c : str)
  {
    *w += character_map[c].w;
    *h = std::max<float> (*h, character_map[c].h);
  }
}
