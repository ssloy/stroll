#include "game.h"

int main() {
    Game game;
    game.init_sdl("stroll stub", 1024, 512, 3);
    while (game.running()) {
        game.handle_events();
        game.draw();
    }
    game.clean();
    return 0;
}

