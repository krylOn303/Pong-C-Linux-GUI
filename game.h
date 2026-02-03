/*
 * Jacob's Pong - Single Player Pong Game
 * Copyright © Stephane Jacob
 * Contact: jacobstephane@outlook.com
 */

#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>

enum GameState { MENU, PLAYING, PAUSED, GAME_OVER };
enum Difficulty { EASY, MEDIUM, HARD };

class Game {
public:
    Game();
    ~Game();
    bool init();
    void run();
    void cleanup();
private:
    void handleEvents();
    void update();
    void render();
    void resetBall(bool playerScored);
    void playSound(int soundId);
    void drawText(int x, int y, std::string text, int scale = 1);
    void drawMenu();
    void drawGameOver();

    SDL_Window* window;
    SDL_Renderer* renderer;
    Mix_Chunk* soundPaddle;
    Mix_Chunk* soundScore;
    Mix_Chunk* soundWall;
    bool running;

    // Game State
    GameState state;
    Difficulty difficulty;
    int menuSelection;

    // Game objects
    SDL_Rect playerPaddle, cpuPaddle, ball;
    int playerScore, cpuScore;
    int ballVelX, ballVelY;
    int cpuVelY;
};

#endif // GAME_H
