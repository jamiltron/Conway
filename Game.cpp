#include "Game.hpp"
#include <boost/algorithm/clamp.hpp>
#include <iostream>
#include <sstream>

static const int SPEED_MIN = 0;
static const int SPEED_MAX = 10;
static const int ZOOM_MIN = 0;
static const int ZOOM_MAX = 8;
static const int SPEED_CONSTANT = 100;

/**
 * Given a string, throw that string and the result of SDL_GetError()
 */
void Game::throwSdlException(std::string message) {
  std::ostringstream oss;
  oss << message << SDL_GetError();
  throw oss.str();
}

Game::Game(unsigned int width, unsigned int height, QuadTree tree)
    : shouldQuit(false), width(width), height(height), tree(tree),
      generationCount(0), speed(5), zoom(4), x(-1), y(-1) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    throwSdlException("Could not initialize SDL: ");
  }

  window =
      SDL_CreateWindow("Conway's Game of Life", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    throwSdlException("Could not create SDL window: ");
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC |
                                                SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    throwSdlException("Could not creat SDL renderer: ");
  }

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_TARGET, width, height);
  if (texture == nullptr) {
    throwSdlException("Could not create SDL texture: ");
  }

  currentTime = SDL_GetPerformanceCounter();
  lastTime = 0;
  timeSinceUpdate = 0;
}

Game::~Game() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyTexture(texture);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int64_t Game::clampMax(int64_t value, unsigned int offset,
                       unsigned int buffer = 1) {
  if (INT64_MAX - offset <= value) {
    return INT64_MAX;
  } else if (INT64_MAX - offset - buffer <= value) {
    return value + offset;
  } else {
    return value + offset + buffer;
  }
}

int64_t Game::clampMin(int64_t value, unsigned int offset,
                       unsigned int buffer = 1) {
  if (INT64_MIN + offset >= value) {
    return INT64_MIN;
  } else if (INT64_MIN + offset + buffer >= value) {
    return value - offset;
  } else {
    return value - offset - buffer;
  }
}

int64_t Game::clampMove(int64_t axis, int amount) const {
  if (amount < 0) {
    return clampMin(axis, -amount, 0);
  } else if (amount > 0) {
    return clampMax(axis, amount, 0);
  } else {
    return axis;
  }
}

void Game::handleZoom(int amount) {
  zoom = boost::algorithm::clamp(zoom + amount, ZOOM_MIN, ZOOM_MAX);
}

void Game::handleSpeedAdjust(int amount) {
  speed = boost::algorithm::clamp(speed + amount, SPEED_MIN, SPEED_MAX);
}

/**
 * Handles all legal input.
 * TODO: ensure we don't allow the camera to roll-over INT64_MIN/max
 */
void Game::handleInput() {
  while (SDL_PollEvent(&event)) {

    if (event.type == SDL_QUIT) {
      shouldQuit = true;
    } else if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
        shouldQuit = true;
        break;
      case SDLK_EQUALS:
        handleSpeedAdjust(-1);
        break;
      case SDLK_MINUS:
        handleSpeedAdjust(1);
        break;
      case SDLK_LEFTBRACKET:
        handleZoom(-1);
        break;
      case SDLK_RIGHTBRACKET:
        handleZoom(1);
        break;
      case SDLK_w:
        y = clampMove(y, -1);
        break;
      case SDLK_s:
        y = clampMove(y, 1);
        break;
      case SDLK_a:
        x = clampMove(x, -1);
        break;
      case SDLK_d:
        x = clampMove(x, 1);
        break;
      default:
        break;
      }
    }
  }
}

/**
 * Update the clock, run the tree's next generation if its time.
 */
void Game::update() {
  lastTime = currentTime;
  currentTime = SDL_GetPerformanceCounter();
  double dt = (double)((currentTime - lastTime) * 1000 /
                       (double)SDL_GetPerformanceFrequency());

  timeSinceUpdate += dt;
  if (timeSinceUpdate > speed * SPEED_CONSTANT) {
    auto ticks = SDL_GetTicks();
    tree.nextGeneration();
    std::cout << "Generation " << ++generationCount << " at height "
              << tree.height() << " took " << SDL_GetTicks() - ticks << "ms"
              << std::endl;
    timeSinceUpdate -= speed * SPEED_CONSTANT;
  }
}

/**
 * Renders a square per cell within the screen.
 */
void Game::render() {
  SDL_SetRenderTarget(renderer, texture);
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);

  unsigned int cellSize = 1 << zoom;
  rect.w = cellSize;
  rect.h = cellSize;

  unsigned int cellWidth = width / cellSize;
  unsigned int cellHeight = height / cellSize;

  unsigned int w = cellWidth / 2;
  unsigned int h = cellHeight / 2;

  int64_t yMin = clampMin(y, h);
  int64_t yMax = clampMax(y, h);
  int64_t xMin = clampMin(x, w);
  int64_t xMax = clampMax(x, w);

  for (int64_t i = yMin; i <= yMax && i >= yMin; i++) {
    for (int64_t j = xMin; j <= xMax && j >= xMin; j++) {
      if (tree.getCellAlive(j, i)) {
        rect.x = (j - x + w) * cellSize;
        rect.y = (i - y + h) * cellSize;

        SDL_RenderDrawRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &rect);
      }
    }
  }

  SDL_SetRenderTarget(renderer, nullptr);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}
