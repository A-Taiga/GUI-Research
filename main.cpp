#include "agui.h"
#include "sdl2_renderer_backend.h"
#include <SDL2/SDL.h>
#include <SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <chrono>
#include <print>
#include <thread>

template<std::size_t FPS>
class Timer
{
    private:
        using duration_type = std::chrono::duration<double, std::ratio<1,FPS>>;
        using clock_type = std::chrono::time_point<std::chrono::steady_clock, std::remove_const_t<duration_type>>;

        static constexpr duration_type time_between_frames{1};
        clock_type tp;

    public:
        Timer(): tp{std::chrono::steady_clock::now()}{}
        void sleep()
        {
            tp += time_between_frames;
            std::this_thread::sleep_until(tp - std::chrono::microseconds(100));
            while(std::chrono::steady_clock::now() < tp) {}
        }
};

template <std::size_t FPS>
class FPS_Counter
{
    private:
        std::atomic<int> frame_count;
        std::atomic<int> frames;
        std::atomic<int> last_frame_count;
        std::chrono::time_point<std::chrono::steady_clock> last_time_point;

    public:
        FPS_Counter(): frame_count{0}, last_frame_count{0}, last_time_point{std::chrono::steady_clock::now()} {}

        int update()
        {

            ++frame_count;
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_time = now - last_time_point;
            if(elapsed_time >= std::chrono::seconds(1))
            {
                frames.store(frame_count);
                frame_count = 0;
                last_time_point = now;
            }
            return frames;
        }
};

int main()
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("AGUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return -1;
    }
    

    AGUI::SDL2::impl_backend (window, renderer);

    AGUI::create_frame ("Test Frame 1", 100, 100, 600, 400);
    AGUI::create_frame ("Test Frame 2", 800, 100, 600, 400);

    AGUI::create_button ("Test Frame 1", "Button", 0, 0);
    AGUI::create_button ("Test Frame 1", "X", 0, 100);

    std::string s{};

    for (int i = 33; i < 127; ++i)
        s += i;
    AGUI::create_label("Test Frame 2", s, 0, 0);




    bool running = true;

    Timer<60> timer;
    FPS_Counter<60> fps_counter;
    
    while (running)
    {
        [[maybe_unused]] const int frames = fps_counter.update();

        SDL_RenderClear(renderer);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            AGUI::SDL2::impl_event (&event);

            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                running = false;
        }

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        AGUI::set_color(AGUI::yellow);
        AGUI::print ({0,0}, "FPS: {}", frames);


        AGUI::update();
        SDL_RenderPresent(renderer);
        timer.sleep();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

