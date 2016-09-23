#ifndef _GAME_H_
#define _GAME_H_

#include <SDL/SDL.h>

class Game {
public:
    Game();
    bool init_sdl(const char* title, int width, int height, int bpp);
    void handle_events();
    void draw();
    void clean();
    bool running();

private:
    bool game_running_;
    int strafe_;
    int walk_;

    SDL_Surface* sdl_screen_;
    int width_;
    int height_;
    int bpp_;
};

#endif

