#include <SDL/SDL.h>
#include <iostream>
#include <cmath>
#include "game.h"

void Game::draw() {
    SDL_FillRect(sdl_screen_, NULL, SDL_MapRGB(sdl_screen_->format, 255, 255, 255));

    int w = sdl_screen_->w/2;
    for (int i=0; i<mapw; i++) { // draw the map
        for (int j=0; j<maph; j++) {
            if (map[i+j*mapw]==' ') continue;
            SDL_Rect tmp;
            for (int x=0; x<32; x++) {
                for (int y=0; y<32; y++) {
                    putpixel(x + i*32+w, y + j*32, getpixel(map[i+j*mapw]-'0', texsize/2, texsize/2));
                }
            }
        }
    }

    for (int i=0; i<w; i++) { // draw the "3D" view + visibility cone
        float ca = (1.-i/float(w)) * (a-fov/2.) + i/float(w)*(a+fov/2.);
        for (float t=0; t<20; t+=.05) {
            float cx = x+cos(ca)*t;
            float cy = y+sin(ca)*t;
            putpixel(w+cx*32, cy*32, 0); // visibility cone at the map

            int idx = int(cx)+int(cy)*mapw;
            if (map[idx]!=' ') {
                int h = sdl_screen_->h/t; // height of current vertical segment to draw
                int tx = std::max(std::abs(cx-floor(cx+.5)), std::abs(cy-floor(cy+.5)))*texsize; // x-texcoord
                int wall_tex = map[idx]-'0';
                for (int ty=0; ty<h; ty++) { // we need to scale texsize to h, thus y-texcoord can be computed as ty*64/h
                    putpixel(i, ty+(sdl_screen_->h-h)/2, getpixel(wall_tex, tx, (ty*64)/h));
                }
                break;
            }
        }
    }

    SDL_Flip(sdl_screen_);
}


Game::Game() : x(3.456), y(3.456), a(0), strafe_(0), walk_(0) {
    game_running_ = false;
    sdl_screen_ = textures_ = NULL;
}

bool Game::init_sdl(const char* title, int width, int height, int bpp) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        std::cerr << "SDL_Init failed, SDL_GetError()=" << SDL_GetError() << std::endl;
        return false;
    }
    SDL_WM_SetCaption(title, NULL);
    sdl_screen_ = SDL_SetVideoMode(width, height, 8*bpp, 0);
    if (!sdl_screen_) {
        std::cerr << "SDL_SetVideoMode failed, SDL_GetError()=" << SDL_GetError() << std::endl;
        return false;
    }
    game_running_ = true;
    width_ = width;
    height_ = height;
    bpp_ = bpp;

    SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_ShowCursor(SDL_DISABLE);

    textures_ = SDL_LoadBMP("walltext.bmp");
    if (!textures_) {
        std::cerr << "Couldn't load texture file " << SDL_GetError() << std::endl;
        return false;
    }
    if (textures_->format->BytesPerPixel!=bpp_) {
        std::cerr << "Game screen bpp does not match textures bpp" << std::endl;
        return false;
    }
    texsize = textures_->h;
    ntextures = textures_->w / texsize;
    if (textures_->w != ntextures*texsize) {
        std::cerr << "Incorrect textures file: its width must be a multiple of its height" << std::endl;
    }
    std::cout << "Successfully read " << ntextures << " textures of " << texsize << "x" << texsize << " pixels" << std::endl;
    
    return true;
}

bool Game::running() {
    return game_running_;
}

void Game::handle_events() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        game_running_ = !(SDL_QUIT==event.type || (SDL_KEYDOWN==event.type && SDLK_ESCAPE==event.key.keysym.sym));
        if (SDL_KEYUP==event.type) {
            if ('a'==event.key.keysym.sym || 'd'==event.key.keysym.sym) strafe_ = 0;
            if ('w'==event.key.keysym.sym || 's'==event.key.keysym.sym) walk_   = 0;
        }
        if (SDL_KEYDOWN==event.type) {
            if ('d'==event.key.keysym.sym) strafe_ =  1;
            if ('a'==event.key.keysym.sym) strafe_ = -1;
            if ('w'==event.key.keysym.sym) walk_ =  1;
            if ('s'==event.key.keysym.sym) walk_ = -1;
        }
        if (event.type == SDL_MOUSEMOTION) {
            a += event.motion.xrel * .01;
        }
    }

    // TODO proper delays

    float nx = x + strafe_*cos(a+M_PI/2)*.1 + walk_*cos(a)*.1;
    float ny = y + strafe_*sin(a+M_PI/2)*.1 + walk_*sin(a)*.1;

    if (int(nx)>=0 && int(nx)<mapw && int(ny)>=0 && int(ny)<maph && map[int(nx)+int(ny)*mapw]==' ') {
        x = nx;
        y = ny;
    }
}

void Game::clean() {
    if (sdl_screen_) SDL_FreeSurface(sdl_screen_);
    if (textures_  ) SDL_FreeSurface(textures_  );
    SDL_Quit();
}

Uint32 Game::getpixel(int itex, int x, int y) {
    int texsize = textures_->h;
    if (itex<0 || itex>=ntextures || x<0 || y<0 || x>=texsize || y>=texsize) return 0;
    Uint8 *p = (Uint8 *)textures_->pixels + y*textures_->pitch + (x+texsize*itex)*bpp_;
    return p[0] | p[1] << 8 | p[2] << 16; // TODO if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
}

void Game::putpixel(int x, int y, Uint32 pixel) {
    if (x<0 || y<0 || x>=sdl_screen_->w || y>=sdl_screen_->h) return;
    Uint8 *p = (Uint8 *)sdl_screen_->pixels + y*sdl_screen_->pitch + x*bpp_;
    for (int i=0; i<bpp_; i++) {
        p[i] = ((Uint8*)&pixel)[i]; // TODO if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    }
}

