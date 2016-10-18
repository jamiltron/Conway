#include "../QuadTreeNode.hpp"
#include "catch.hpp"

TEST_CASE("QuadTreeNode population sizes", "[QuadTreeNode]") {
  SECTION("Empty nodes have a population of 0") {
    auto node = QuadTreeNode::retrieve(false);
    REQUIRE(0 == node->population);
  }

  SECTION("Alive nodes with no children have a population of 1") {
    auto node = QuadTreeNode::retrieve(true);
    REQUIRE(1 == node->population);
  }

  SECTION("Nodes with children have a population equal to their children") {
    auto alive = QuadTreeNode::retrieve(true);
    auto empty = QuadTreeNode::retrieve(false);
    auto zero = QuadTreeNode::retrieve(empty, empty, empty, empty);
    auto one = QuadTreeNode::retrieve(alive, empty, empty, empty);
    auto two = QuadTreeNode::retrieve(alive, alive, empty, empty);
    auto three = QuadTreeNode::retrieve(alive, alive, alive, empty);
    auto four = QuadTreeNode::retrieve(alive, alive, alive, alive);
    auto eight = QuadTreeNode::retrieve(zero, one, three, four);

    REQUIRE(1 == alive->population);
    REQUIRE(0 == empty->population);
    REQUIRE(0 == zero->population);
    REQUIRE(1 == one->population);
    REQUIRE(2 == two->population);
    REQUIRE(3 == three->population);
    REQUIRE(4 == four->population);
    REQUIRE(8 == eight->population);
  }
}

TEST_CASE("QuadTreeNode (get/set)CellAlive", "[QuadTreeNode]") {
  SECTION("All empty cells should return false") {
    auto empty = QuadTreeNode::retrieve(false);

    auto root = QuadTreeNode::retrieve(empty, empty, empty, empty);

    bool tl = root->getCellAlive(0, 0);
    bool tr = root->getCellAlive(-1, 0);
    bool bl = root->getCellAlive(0, -1);
    bool br = root->getCellAlive(-1, -1);

    REQUIRE(1 == root->height);
    REQUIRE(false == tl);
    REQUIRE(false == tr);
    REQUIRE(false == bl);
    REQUIRE(false == br);
  }

  SECTION("Alive cells should return true, false otherwise") {
    auto empty = QuadTreeNode::retrieve(false);
    auto full = QuadTreeNode::retrieve(true);

    auto root = QuadTreeNode::retrieve(empty, full, empty, full);

    bool tl = root->getCellAlive(-1, -1);
    bool tr = root->getCellAlive(0, -1);
    bool bl = root->getCellAlive(-1, 0);
    bool br = root->getCellAlive(0, 0);

    REQUIRE(false == tl);
    REQUIRE(true == tr);
    REQUIRE(false == bl);
    REQUIRE(true == br);
  }

  SECTION("Inserted cells should return true") {
    auto empty = QuadTreeNode::retrieve(false);

    auto root1 = QuadTreeNode::retrieve(empty, empty, empty, empty);

    auto root2 = root1->setCellAlive(-1, 0);
    auto root3 = root2->setCellAlive(0, -1);

    bool tl1 = root1->getCellAlive(0, 0);
    bool tr1 = root1->getCellAlive(-1, 0);
    bool bl1 = root1->getCellAlive(0, -1);
    bool br1 = root1->getCellAlive(-1, -1);

    bool tl2 = root2->getCellAlive(0, 0);
    bool tr2 = root2->getCellAlive(-1, 0);
    bool bl2 = root2->getCellAlive(0, -1);
    bool br2 = root2->getCellAlive(-1, -1);

    bool tl3 = root3->getCellAlive(0, 0);
    bool tr3 = root3->getCellAlive(-1, 0);
    bool bl3 = root3->getCellAlive(0, -1);
    bool br3 = root3->getCellAlive(-1, -1);

    REQUIRE(false == tl1);
    REQUIRE(false == tr1);
    REQUIRE(false == bl1);
    REQUIRE(false == br1);

    REQUIRE(false == tl2);
    REQUIRE(true == tr2);
    REQUIRE(false == bl2);
    REQUIRE(false == br2);

    REQUIRE(false == tl3);
    REQUIRE(true == tr3);
    REQUIRE(true == bl3);
    REQUIRE(false == br3);
  }
}

TEST_CASE("compact", "[QuadTreeNode]") {
  SECTION("compact on empty node produces an emtpy node") {
    auto empty = QuadTreeNode::retrieve(false);
    REQUIRE(empty == empty->compact());
  }

  SECTION("compact on an emptyAtLevel(1) node produces that node") {
    auto node = QuadTreeNode::createEmptyAtHeight(1);
    REQUIRE(node == node->compact());
  }

  SECTION("compact on an emptyAtLevel(10) node produces an empty 2x2 node") {
    auto twoByTwo = QuadTreeNode::createEmptyAtHeight(1);
    auto node = QuadTreeNode::createEmptyAtHeight(10);
    REQUIRE(twoByTwo == node->compact());
  }

  SECTION("compact on an 8x8 node with a live child on the border produces "
          "that node") {
    auto emptyNode = QuadTreeNode::createEmptyAtHeight(4);
    auto node = emptyNode->setCellAlive(-7, 8);
    auto compacted = node->compact();

    REQUIRE(node->population == 1);
    REQUIRE(node->population == compacted->population);
    REQUIRE(node == compacted);
  }

  SECTION("compact on an 8x8 node with a live child in the middle produces "
          "that middle") {
    auto empty = QuadTreeNode::retrieve(false);
    auto full = QuadTreeNode::retrieve(true);
    auto node = QuadTreeNode::createEmptyAtHeight(4);
    node = node->setCellAlive(0, -1);
    node = node->setCellAlive(-1, 0);
    auto compacted = node->compact();

    REQUIRE(2 == compacted->population);
    REQUIRE(node->population == compacted->population);
    REQUIRE(1 == compacted->height);

    REQUIRE(empty == compacted->nw);
    REQUIRE(full == compacted->ne);
    REQUIRE(full == compacted->sw);
    REQUIRE(empty == compacted->se);
  }

  SECTION("compact on an 8x8 node with a live child in the middle produces "
          "the smallest node with that child in it") {
    auto empty = QuadTreeNode::retrieve(false);
    auto full = QuadTreeNode::retrieve(true);
    auto node = QuadTreeNode::createEmptyAtHeight(4);
    node = node->setCellAlive(0, 1);
    auto compacted = node->compact();

    REQUIRE(1 == compacted->population);
    REQUIRE(node->population == compacted->population);
    REQUIRE(2 == compacted->height);

    REQUIRE(empty == compacted->nw->ne);

    REQUIRE(empty == compacted->nw->nw);
    REQUIRE(empty == compacted->nw->sw);
    REQUIRE(empty == compacted->nw->se);

    REQUIRE(empty == compacted->ne->nw);
    REQUIRE(empty == compacted->ne->ne);
    REQUIRE(empty == compacted->ne->sw);
    REQUIRE(empty == compacted->ne->se);

    REQUIRE(empty == compacted->sw->nw);
    REQUIRE(empty == compacted->sw->ne);
    REQUIRE(empty == compacted->sw->sw);
    REQUIRE(empty == compacted->sw->se);

    REQUIRE(empty == compacted->se->nw);
    REQUIRE(empty == compacted->se->ne);
    REQUIRE(full == compacted->se->sw);
    REQUIRE(empty == compacted->se->se);
  }
}

TEST_CASE("createEmptyAtHeight", "[QuadTreeNode]") {
  SECTION("emptyAtLevel(0) produces an emtpy node") {
    auto node = QuadTreeNode::createEmptyAtHeight(0);
    REQUIRE(0 == node->alive);
    REQUIRE(0 == node->population);
  }

  SECTION("emptyAtLevel(63) works") {
    auto node = QuadTreeNode::createEmptyAtHeight(63);
    REQUIRE(0 == node->alive);
    REQUIRE(0 == node->population);
    REQUIRE(63 == node->height);
  }

  SECTION("emptyAtLevel(1) produces a node containing 4 empty leaves") {
    auto empty = QuadTreeNode::retrieve(false);
    auto node = QuadTreeNode::createEmptyAtHeight(1);

    REQUIRE(0 == node->population);
    REQUIRE(empty == node->nw);
    REQUIRE(empty == node->ne);
    REQUIRE(empty == node->sw);
    REQUIRE(empty == node->se);
  }

  SECTION("createEmptyAtHeight(2) produces a node containing 4 nodes each "
          "containing 4 empty leaves") {
    auto empty = QuadTreeNode::retrieve(false);
    auto node = QuadTreeNode::createEmptyAtHeight(2);

    REQUIRE(0 == node->population);
    REQUIRE(empty == node->nw->nw);
    REQUIRE(empty == node->nw->ne);
    REQUIRE(empty == node->nw->sw);
    REQUIRE(empty == node->nw->se);

    REQUIRE(empty == node->ne->nw);
    REQUIRE(empty == node->ne->ne);
    REQUIRE(empty == node->ne->sw);
    REQUIRE(empty == node->ne->se);

    REQUIRE(empty == node->sw->nw);
    REQUIRE(empty == node->sw->ne);
    REQUIRE(empty == node->sw->sw);
    REQUIRE(empty == node->sw->se);

    REQUIRE(empty == node->se->nw);
    REQUIRE(empty == node->se->ne);
    REQUIRE(empty == node->se->sw);
    REQUIRE(empty == node->se->se);
  }
}
