#include "QuadTreeNode.hpp"

std::unordered_map<QuadTreeNode, QuadTreeNode *> QuadTreeNode::cache =
    std::unordered_map<QuadTreeNode, QuadTreeNode *>();

/**
 * Creates a new leaf node.
 * @param alive if this node is living or dead
 */
QuadTreeNode::QuadTreeNode(const bool alive)
    : nw(nullptr), ne(nullptr), sw(nullptr), se(nullptr), alive(alive),
      height(0), population(alive ? 1 : 0) {
  next = nullptr;
}

/**
 * Creates a new non-leaf node composed of four child nodes, height being one
 * level up from the child nodes, population being the total population of the
 * children, and alive if any child node contains any living cells.
 * @param nw, ne, sw, se the four child nodes.
 */
QuadTreeNode::QuadTreeNode(QuadTreeNode *nw, QuadTreeNode *ne, QuadTreeNode *sw,
                           QuadTreeNode *se)
    : nw(nw), ne(ne), sw(sw), se(se),
      alive(nw->alive || ne->alive || sw->alive || se->alive),
      height(nw->height + 1), population(nw->population + ne->population +
                                         sw->population + se->population) {
  next = nullptr;
}

/**
 * Equality operator. Note that this does not care about next.
 */
bool QuadTreeNode::operator==(const QuadTreeNode &other) const {
  if (height != other.height) {
    return false;
  }

  if (height == 0) {
    return alive == other.alive;
  }

  return nw == other.nw && ne == other.ne && sw == other.sw && se == other.se;
}

bool QuadTreeNode::operator!=(const QuadTreeNode &other) const {
  return !(*this == other);
}

/**
 * Static method returning an empty (all dead) node all the way down to the
 * leaves (height 0).
 * @param the height at which to start building the empty nodes downwards.
 */
QuadTreeNode *QuadTreeNode::createEmptyAtHeight(unsigned int height) {
  if (height == 0) {
    return retrieve(false);
  }
  auto node = createEmptyAtHeight(height - 1);
  return retrieve(node, node, node, node);
}

/**
 * Given a quad tree node, look up the relevant pointer in the cache, or create
 * a new one, store it in the cache, and return it.
 */
QuadTreeNode *const QuadTreeNode::intern(QuadTreeNode node) {
  auto ret = cache[node];

  if (!ret) {
    ret = new QuadTreeNode(node);
    cache[node] = ret;
  }

  return ret;
}

/**
 * Create a return a new leaf node from the cache.
 */
QuadTreeNode *const QuadTreeNode::retrieve(bool alive) {
  return intern(QuadTreeNode(alive));
}

/**
 * Create or return a new quad from the cache.
 */
QuadTreeNode *const QuadTreeNode::retrieve(QuadTreeNode *nw, QuadTreeNode *ne,
                                           QuadTreeNode *sw, QuadTreeNode *se) {
  return intern(QuadTreeNode(nw, ne, sw, se));
}

/**
 * Returns whether or not the portions of the quad along the border of this
 * quad's center are all dead.
 */
bool QuadTreeNode::areBordersEmpty() const {
  return
      // check that everything but nw->se is empty
      !nw->nw->alive && !nw->ne->alive && !nw->sw->alive &&
      // check that everything but ne->sw is empty
      !ne->nw->alive && !ne->ne->alive && !ne->se->alive &&
      // check that everything but sw->ne is empty
      !sw->nw->alive && !sw->sw->alive && !sw->se->alive &&
      // check that everything but se->nw is empty
      !se->ne->alive && !se->sw->alive && !se->se->alive;
}

/**
 * Compacts a given node by recursively returning this quad's center until the
 * remaining borders are not empty, also ensuring that the node's height is
 * less than the maximum height.
 */
QuadTreeNode *const QuadTreeNode::compact() const {
  if (height <= 1) {
    return const_cast<QuadTreeNode *const>(this);
  }

  auto node = QuadTreeNode::retrieve(nw, ne, sw, se);

  while (node->height > MAX_HEIGHT ||
         (node->height >= MIN_GROWABLE && node->areBordersEmpty())) {
    node = retrieve(node->nw->se, node->ne->sw, node->sw->ne, node->se->nw);
  }
  return node;
}

/**
 * Returns the life state of a cell at coordinate (x,y)
 */
bool QuadTreeNode::getCellAlive(int64_t x, int64_t y) const {
  if (height == 0) {
    return alive;
  }

  // figure out how far off from center we are, so we can traverse into the
  // correct node
  int64_t offset = getSeekOffset();

  if (x < 0) {
    if (y < 0) {
      if (nw->alive) {
        return nw->getCellAlive(x + offset, y + offset);
      } else {
        return false;
      }
    } else {
      if (sw->alive) {
        return sw->getCellAlive(x + offset, y - offset);
      } else {
        return false;
      }
    }
  } else {
    if (y < 0) {
      if (ne->alive) {
        return ne->getCellAlive(x - offset, y + offset);
      } else {
        return false;
      }
    } else {
      if (se->alive) {
        return se->getCellAlive(x - offset, y - offset);
      } else {
        return false;
      }
    }
  }
}

/**
 * Returns the offset needed to calculate cell get/sets.
 */
int64_t QuadTreeNode::getSeekOffset() const {
  // since we can only input within signed 64-bit, we limit the offset height to
  // the max height
  unsigned int thisHeight = height <= MAX_HEIGHT ? height : MAX_HEIGHT;
  // calculate how far off from the center we are (in terms of fourths, which is
  // why we shift by -2)
  int64_t shiftBy = thisHeight - 2 <= 0 ? 0 : thisHeight - 2;
  return int64_t(1) << shiftBy;
}

/**
 * Returns a new node that has been "grown" one level higher, with empty quads
 * accounting for all the new space.
 */
QuadTreeNode *const QuadTreeNode::grow() const {
  auto empty = QuadTreeNode::createEmptyAtHeight(height - 1);
  auto newNW = retrieve(empty, empty, empty, nw);
  auto newNE = retrieve(empty, empty, ne, empty);
  auto newSW = retrieve(empty, sw, empty, empty);
  auto newSE = retrieve(se, empty, empty, empty);

  return retrieve(newNW, newNE, newSW, newSE);
}

/**
 * Return the next generation of this node's center.
 */
QuadTreeNode *const QuadTreeNode::nextCenter() const {
  return retrieve(nw->se, ne->sw, sw->ne, se->nw)->nextGeneration();
}

/**
 * Return the center 2^(height - 1) square center of this node
 * forward in time one generation. This is done by recursively calculating the
 * centered node all the way down to the 2^1 square center. This method
 * short-circuits if the given node has a memoized version of its next
 * generation, or if its population is 0.
 */
QuadTreeNode *const QuadTreeNode::nextGeneration() {
  if (next != nullptr) {
    return next;
  }

  // skip empty regions quickly
  if (population == 0) {
    next = nw;
    return next;
  }

  // the bottom case - calculate the next living state of the inner center
  if (height == MIN_GROWABLE) {
    int64_t nwNeighbors = nw->population - nw->se->population +
                          ne->populationWest() + sw->populationNorth() +
                          se->nw->population;

    int64_t neNeighbors = ne->population - ne->sw->population +
                          nw->populationEast() + se->populationNorth() +
                          sw->ne->population;

    int64_t swNeighbors = sw->population - sw->ne->population +
                          se->populationWest() + nw->populationSouth() +
                          ne->sw->population;

    int64_t seNeighbors = se->population - se->nw->population +
                          sw->populationEast() + ne->populationSouth() +
                          nw->se->population;

    // this checks that a dead cell has 3 neighbors, or a living cell has 2
    bool nwLives =
        nwNeighbors == 3 || (nwNeighbors == 2 && nw->se->population > 0);
    bool neLives =
        neNeighbors == 3 || (neNeighbors == 2 && ne->sw->population > 0);
    bool swLives =
        swNeighbors == 3 || (swNeighbors == 2 && sw->ne->population > 0);
    bool seLives =
        seNeighbors == 3 || (seNeighbors == 2 && se->nw->population > 0);

    auto newNW = retrieve(nwLives);
    auto newNE = retrieve(neLives);
    auto newSW = retrieve(swLives);
    auto newSE = retrieve(seLives);

    next = retrieve(newNW, newNE, newSW, newSE);
    return next;
  }

  // break the center of the current node into 9 sub-squares:
  // n00 | n01 | n02
  // ----+-----+----
  // n10 | n11 | n12
  // ----+-----+----
  // n20 | n21 | n22
  auto n00 = nw->nextGeneration();
  auto n01 = nextHorizontal(nw, ne);
  auto n02 = ne->nextGeneration();
  auto n10 = nextVertical(nw, sw);
  auto n11 = nextCenter();
  auto n12 = nextVertical(ne, se);
  auto n20 = sw->nextGeneration();
  auto n21 = nextHorizontal(sw, se);
  auto n22 = se->nextGeneration();

  // use the above 9 squares to get the four inner quads, representing the
  // 2^(height - 1) center of this node forward one generation, look at the
  // comment above to get a visual representation of what each of thse are
  // calculating
  auto nextNW = QuadTreeNode(n00, n01, n10, n11).retrieveCenteredChildren();
  auto nextNE = QuadTreeNode(n01, n02, n11, n12).retrieveCenteredChildren();
  auto nextSW = QuadTreeNode(n10, n11, n20, n21).retrieveCenteredChildren();
  auto nextSE = QuadTreeNode(n11, n12, n21, n22).retrieveCenteredChildren();

  next = retrieve(nextNW, nextNE, nextSW, nextSE);
  return next;
}

/**
 * Given two quads representing a left and right sides of a center, return the
 * next generation of their merged center.
 */
QuadTreeNode *const QuadTreeNode::nextHorizontal(QuadTreeNode *const west,
                                                 QuadTreeNode *const east) {
  return retrieve(west->ne, east->nw, west->se, east->sw)->nextGeneration();
}

/**
 * Given two quads representing the top and bottom sides of a center, return
 * the next generation of their merged center.
 */
QuadTreeNode *const QuadTreeNode::nextVertical(QuadTreeNode *north,
                                               QuadTreeNode *south) {

  return retrieve(north->sw, north->se, south->nw, south->ne)->nextGeneration();
}

/**
 * Returns the population from the two eastern quads.
 */
uint64_t QuadTreeNode::populationEast() const {
  return ne->population + se->population;
}

/**
 * Returns the population form the two northern quads.
 */
uint64_t QuadTreeNode::populationNorth() const {
  return nw->population + ne->population;
}

/**
 * Returns the population from the two southern quads.
 */
uint64_t QuadTreeNode::populationSouth() const {
  return sw->population + se->population;
}

/**
 * Returns the population from the two western quads.
 */
uint64_t QuadTreeNode::populationWest() const {
  return nw->population + sw->population;
}

/**
 * Returns a new node one level down from the current node, composed of all
 * of the childrens' centers.
 */
QuadTreeNode *const QuadTreeNode::retrieveCenteredChildren() const {
  return retrieve(nw->se, ne->sw, sw->ne, se->nw);
}

/**
 * Returns whether or not a given cell is alive or dead.
 */
QuadTreeNode *const QuadTreeNode::setCellAlive(int64_t x, int64_t y) const {
  if (height == 0) {
    return retrieve(true);
  }

  // calculate how far off from the center we are so we can traverse to the
  // correct node
  int64_t offset = getSeekOffset();

  if (x < 0) {
    if (y < 0) {
      return retrieve(nw->setCellAlive(x + offset, y + offset), ne, sw, se);
    } else {
      return retrieve(nw, ne, sw->setCellAlive(x + offset, y - offset), se);
    }
  } else {
    if (y < 0) {
      return retrieve(nw, ne->setCellAlive(x - offset, y + offset), sw, se);
    } else {
      return retrieve(nw, ne, sw, se->setCellAlive(x - offset, y - offset));
    }
  }
}
