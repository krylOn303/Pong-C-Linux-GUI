/*
 * Jacob's Pong - Single Player Pong Game
 * Copyright © Stephane Jacob
 * Contact: jacobstephane@outlook.com
 */

#include "game.h"
#include <cstdio>

int main() {
    Game game;
    if (!game.init()) {
        printf("Failed to initialize Jacob's Pong.\n");
        return 1;
    }
    game.run();
    game.cleanup();
    printf("Thanks for playing Jacob's Pong!\n");
    return 0;
}
