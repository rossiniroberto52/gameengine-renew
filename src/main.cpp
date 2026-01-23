#include "Game.hpp"

int main(int argc, char* argv[]) {
    Game game;

    // Tenta iniciar. Se falhar, fecha.
    if (!game.init("Minha Engine C++", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false)) {
        return -1;
    }

    // Roda o loop
    game.run();

    return 0;
}