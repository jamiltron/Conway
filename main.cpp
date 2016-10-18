#include "Game.hpp"
#include "InputParser.hpp"
#include "QuadTree.hpp"
#include "QuadTreeNode.hpp"
#include <SDL2/SDL.h>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

int main(int argc, char *argv[]) {
  vector<pair<int64_t, int64_t>> points;

  try {
    points = InputParser::getPoints(argc, argv);
  } catch (const char *e) {
    cout << e << endl;
    cout << "Usage: ./conway {(x0, y0) (x1, y1) ... (xN, yN)} | {-f file.life}"
         << endl;
    return -1;
  }

  QuadTree tree = QuadTree(points);
  Game *game;
  try {
    game = new Game(WIDTH, HEIGHT, tree);
  } catch (const char *e) {
    cout << e;
    return -1;
  }

  cout << "# CONWAY'S GAME OF LIFE" << endl;
  cout << "  * WASD to move the camera." << endl;
  cout << "  * Left bracket to zoom out, right bracket to zoom in." << endl;
  cout << "  * Minus to slow down the simulation, equals to speed it up."
       << endl;
  cout << "  * Escape to quit." << endl;

  while (!game->shouldQuit) {
    game->handleInput();
    game->update();
    game->render();
  }

  return 0;
}
