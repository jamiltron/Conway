#ifndef GAME_HPP
#define GAME_HPP
#include "QuadTree.hpp"
#include <SDL2/SDL.h>

class Game {
public:
  Game(unsigned int, unsigned int, QuadTree);
  ~Game();

  void handleInput();
  void render();
  void update();

  bool shouldQuit;

private:
  static void throwSdlException(std::string);

  void handleZoom(int);
  void handleSpeedAdjust(int);

  const int width;
  const int height;

  QuadTree tree;
  unsigned int generationCount;
  int speed;
  int zoom;
  int64_t x;
  int64_t y;
  Uint64 currentTime;
  Uint64 lastTime;
  double timeSinceUpdate;

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  SDL_Event event;
  SDL_Rect rect;
};

#endif // GAME_HPP
