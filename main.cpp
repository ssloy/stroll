#include "game.h"

int main(int argc, char* args[]) {
    Game game;
    game.init_sdl("walkup stub", 1024, 512, 24);
    while (game.running()) {
        game.handle_events();
        game.draw();
    }
    game.clean();
    return 0;
}

