#include "QuadTree.hpp"

QuadTree::QuadTree(std::vector<std::pair<int64_t, int64_t>> cells) {
  root = QuadTreeNode::createEmptyAtHeight(1);
  updatePoints();
  for (auto const &point : cells) {
    setCellAlive(point.first, point.second);
  }
}

QuadTree::QuadTree()
    : QuadTree(QuadTreeNode::retrieve(
          QuadTreeNode::retrieve(false), QuadTreeNode::retrieve(false),
          QuadTreeNode::retrieve(false), QuadTreeNode::retrieve(false))) {
  updatePoints();
}

QuadTree::QuadTree(QuadTreeNode *quadTreeNode) : root(quadTreeNode) {
  updatePoints();
}

/**
 * Return whether a point is alive or not.
 */
bool QuadTree::getCellAlive(const int64_t x, const int64_t y) {
  if (min <= x && x <= max && min <= y && y <= max) {
    return root->getCellAlive(x, y);
  } else {
    return false;
  }
}

/**
 * Grow the root one additional level, update the points afterward.
 */
void QuadTree::growTree(unsigned int amount) {
  for (int i = 0; i < amount; i++) {
    root = root->grow();
  }
  updatePoints();
}

/**
 * Convenience method to return the root's height.
 */
unsigned int QuadTree::height() { return root->height; }

/**
 * Performs the main calculation, setting the root to be the next generation.
 * See the method in QuadTreeNode's implementation file for more information.
 */
void QuadTree::nextGeneration() {
  // by growing twice we are ensuring we have a center node with equal empty
  // borders to its size allowing for expansion in nextGeneration
  growTree(2);
  root = root->nextGeneration()->compact();
  updatePoints();
}

/**
 * Convenience method to return root's population.
 */
uint64_t QuadTree::population() { return root->population; }

/**
 * Update the min and max variables to relate to the height. This performs some
 * checks to prevent integer roll-over.
 */
void QuadTree::updatePoints() {
  if (root->height >= QuadTreeNode::MAX_HEIGHT) {
    max = INT64_MAX;
    min = INT64_MIN;
  } else {
    int64_t point = root->height <= 0 ? 0 : int64_t(1) << (root->height - 1);

    max = point - 1;
    min = -point;
  }
}

/**
 * Set a cell alive, growing the tree until the point exists within the tree.
 */
void QuadTree::setCellAlive(int64_t x, int64_t y) {
  while (true && root->height <= QuadTreeNode::MAX_HEIGHT) {
    if (min <= x && x <= max && min <= y && y <= max) {
      break;
    }
    growTree(1);
  }
  root = root->setCellAlive(x, y);
}
