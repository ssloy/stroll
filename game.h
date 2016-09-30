#ifndef _GAME_H_
#define _GAME_H_

#include <cmath>
#include <SDL/SDL.h>

const float fov = M_PI/3;
const int mapw = 16;
const int maph = 16;
const char map[maph*mapw+1] = "\
0000222222220000\
1              0\
1      11111   0\
1     0        0\
0     0  1110000\
0     3        0\
0   10000      0\
0   0   11100  0\
0   0   0      0\
0   0   1  00000\
0       1      0\
2       1      0\
0       0      0\
0 0000000      0\
0              0\
0002222222200000";

class Game {
public:
    Game();
    bool init_sdl(const char* title, int width, int height, int bpp);
    void handle_events();
    void draw();
    void clean();
    bool running();

private:
    void putpixel(int x, int y, Uint32 pixel);
    Uint32 getpixel(int itex, int x, int y);

    bool game_running_;

    float x,y,a;    

    int strafe_;
    int walk_;

    SDL_Surface* sdl_screen_;
    SDL_Surface* textures_;
    int ntextures;
    int texsize;

    int width_;
    int height_;
    int bpp_;
};

#endif

