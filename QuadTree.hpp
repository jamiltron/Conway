#ifndef QUADTREE_HPP
#define QUADTREE_HPP
#include "QuadTreeNode.hpp"
#include <cstdint>
#include <utility>
#include <vector>

class QuadTree {
public:
  int64_t min;
  int64_t max;
  QuadTreeNode *root;

  QuadTree();
  QuadTree(QuadTreeNode *);
  QuadTree(std::vector<std::pair<int64_t, int64_t>>);

  bool getCellAlive(int64_t, int64_t);
  void growTree(unsigned int);
  unsigned int height();
  void nextGeneration();
  uint64_t population();
  void setCellAlive(int64_t, int64_t);
  void updatePoints();
};

#endif // QUADTREE_HPP
