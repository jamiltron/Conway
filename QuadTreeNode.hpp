#ifndef QUADTREENODE_HPP
#define QUADTREENODE_HPP
#include <cstdint>
#include <unordered_map>

class QuadTreeNode {
public:
  static QuadTreeNode *createEmptyAtHeight(unsigned int);
  static const unsigned int MAX_HEIGHT = 64;
  static const unsigned int MIN_GROWABLE = 2;

  QuadTreeNode *const nw;
  QuadTreeNode *const ne;
  QuadTreeNode *const sw;
  QuadTreeNode *const se;

  const bool alive;
  const unsigned int height; // distance from leaves
  const uint64_t population; // amount of living nodes lower down the tree

  QuadTreeNode(bool);
  QuadTreeNode(QuadTreeNode *const, QuadTreeNode *const, QuadTreeNode *const,
               QuadTreeNode *const);

  bool operator==(const QuadTreeNode &) const;
  bool operator!=(const QuadTreeNode &) const;

  static QuadTreeNode *const retrieve(bool);
  static QuadTreeNode *const retrieve(QuadTreeNode *const, QuadTreeNode *const,
                                      QuadTreeNode *const, QuadTreeNode *const);

  QuadTreeNode *const compact() const;
  bool getCellAlive(int64_t, int64_t) const;
  QuadTreeNode *const grow() const;
  QuadTreeNode *const nextGeneration();
  QuadTreeNode *const setCellAlive(int64_t, int64_t) const;

private:
  static std::unordered_map<QuadTreeNode, QuadTreeNode *> cache;

  static QuadTreeNode *const intern(QuadTreeNode);

  bool areBordersEmpty() const;
  int64_t getSeekOffset() const;

  QuadTreeNode *next;

  uint64_t populationWest() const;
  uint64_t populationEast() const;
  uint64_t populationNorth() const;
  uint64_t populationSouth() const;

  static QuadTreeNode *const nextHorizontal(QuadTreeNode *const,
                                            QuadTreeNode *const);

  static QuadTreeNode *const nextVertical(QuadTreeNode *const,
                                          QuadTreeNode *const);

  QuadTreeNode *const retrieveCenteredChildren() const;
  QuadTreeNode *const nextCenter() const;
};

/**
 * Injects the hash function for QuadTreeNode into the standard namespace,
 * advice taken from StackOverflow (TODO: refind answer to link from here).
 */
namespace std {
template <> struct hash<QuadTreeNode> {
  size_t operator()(const QuadTreeNode &node) const {
    if (node.height == 0) {
      return hash<uint64_t>()(node.population);
    }
    return 29 * (node.nw->alive ? hash<QuadTreeNode>()(*node.nw) : 0) +
           31 * (node.ne->alive ? hash<QuadTreeNode>()(*node.ne) : 0) +
           37 * (node.sw->alive ? hash<QuadTreeNode>()(*node.sw) : 0) +
           41 * (node.se->alive ? hash<QuadTreeNode>()(*node.se) : 0);
  }
};
}

#endif // QUADTREENODE_HPP
