/*
 * Jacob's Pong - Single Player Pong Game
 * Copyright © Stephane Jacob
 * Contact: jacobstephane@outlook.com
 */

#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 100
#define BALL_SIZE 20
#define PADDLE_SPEED 8
#define BALL_SPEED 7
#define CPU_SPEED 6
#define SCORE_LIMIT 10

Game::Game() : window(nullptr), renderer(nullptr), soundPaddle(nullptr), soundScore(nullptr), soundWall(nullptr), running(false), playerScore(0), cpuScore(0), ballVelX(0), ballVelY(0), cpuVelY(0) {
    srand((unsigned int)time(nullptr));
}

Game::~Game() {
    cleanup();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }
    window = SDL_CreateWindow("Jacob's Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    // Generate simple beeps for sounds
    soundPaddle = Mix_QuickLoad_RAW((Uint8*)"\x00\xFF\x00\xFF", 2);
    soundScore = Mix_QuickLoad_RAW((Uint8*)"\xFF\x00\xFF\x00", 2);
    soundWall = Mix_QuickLoad_RAW((Uint8*)"\x80\x80\x80\x80", 2);
    // Init paddles and ball
    playerPaddle = {30, WINDOW_HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT};
    cpuPaddle = {WINDOW_WIDTH-50, WINDOW_HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT};
    ball = {WINDOW_WIDTH/2 - BALL_SIZE/2, WINDOW_HEIGHT/2 - BALL_SIZE/2, BALL_SIZE, BALL_SIZE};
    resetBall(rand()%2);
    playerScore = cpuScore = 0;
    running = true;
    return true;
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void Game::cleanup() {
    if (soundPaddle) Mix_FreeChunk(soundPaddle);
    if (soundScore) Mix_FreeChunk(soundScore);
    if (soundWall) Mix_FreeChunk(soundWall);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    Mix_CloseAudio();
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = false;
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }
    }
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_UP]) playerPaddle.y -= PADDLE_SPEED;
    if (keystate[SDL_SCANCODE_DOWN]) playerPaddle.y += PADDLE_SPEED;
    if (playerPaddle.y < 0) playerPaddle.y = 0;
    if (playerPaddle.y > WINDOW_HEIGHT - PADDLE_HEIGHT) playerPaddle.y = WINDOW_HEIGHT - PADDLE_HEIGHT;
}

void Game::update() {
    // CPU AI
    if (ball.y + BALL_SIZE/2 < cpuPaddle.y + PADDLE_HEIGHT/2) cpuPaddle.y -= CPU_SPEED;
    else if (ball.y + BALL_SIZE/2 > cpuPaddle.y + PADDLE_HEIGHT/2) cpuPaddle.y += CPU_SPEED;
    if (cpuPaddle.y < 0) cpuPaddle.y = 0;
    if (cpuPaddle.y > WINDOW_HEIGHT - PADDLE_HEIGHT) cpuPaddle.y = WINDOW_HEIGHT - PADDLE_HEIGHT;
    // Ball movement
    ball.x += ballVelX;
    ball.y += ballVelY;
    // Collisions
    if (ball.y <= 0 || ball.y >= WINDOW_HEIGHT - BALL_SIZE) {
        ballVelY = -ballVelY;
        playSound(2);
    }
    if (SDL_HasIntersection(&ball, &playerPaddle)) {
        ballVelX = abs(ballVelX);
        playSound(0);
    }
    if (SDL_HasIntersection(&ball, &cpuPaddle)) {
        ballVelX = -abs(ballVelX);
        playSound(0);
    }
    // Score
    if (ball.x < 0) {
        cpuScore++;
        playSound(1);
        resetBall(false);
    }
    if (ball.x > WINDOW_WIDTH - BALL_SIZE) {
        playerScore++;
        playSound(1);
        resetBall(true);
    }
    // Win condition
    if (playerScore >= SCORE_LIMIT || cpuScore >= SCORE_LIMIT) running = false;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // Draw paddles and ball
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &playerPaddle);
    SDL_RenderFillRect(renderer, &cpuPaddle);
    SDL_RenderFillRect(renderer, &ball);
    // Draw net
    for (int y = 0; y < WINDOW_HEIGHT; y += 40) {
        SDL_Rect net = {WINDOW_WIDTH/2 - 2, y, 4, 20};
        SDL_RenderFillRect(renderer, &net);
    }
    // Draw scores (simple rectangles for demo)
    for (int i = 0; i < playerScore; ++i) {
        SDL_Rect s = {50 + i*15, 30, 10, 20};
        SDL_RenderFillRect(renderer, &s);
    }
    for (int i = 0; i < cpuScore; ++i) {
        SDL_Rect s = {WINDOW_WIDTH-60 - i*15, 30, 10, 20};
        SDL_RenderFillRect(renderer, &s);
    }
    SDL_RenderPresent(renderer);
}

void Game::resetBall(bool playerScored) {
    ball.x = WINDOW_WIDTH/2 - BALL_SIZE/2;
    ball.y = WINDOW_HEIGHT/2 - BALL_SIZE/2;
    ballVelX = (playerScored ? 1 : -1) * BALL_SPEED;
    ballVelY = (rand()%2 ? 1 : -1) * (BALL_SPEED - 2 + rand()%5);
}

void Game::playSound(int soundId) {
    switch (soundId) {
        case 0: if (soundPaddle) Mix_PlayChannel(-1, soundPaddle, 0); break;
        case 1: if (soundScore) Mix_PlayChannel(-1, soundScore, 0); break;
        case 2: if (soundWall) Mix_PlayChannel(-1, soundWall, 0); break;
    }
}
