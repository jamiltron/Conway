#include "../QuadTree.hpp"
#include "catch.hpp"
#include <cstdint>
#include <utility>
#include <vector>
using namespace std;

TEST_CASE("QuadTree initialization", "[QuadTree]") {
  SECTION("Single-cell quad trees should report dead at (0,0)") {
    QuadTree tree = QuadTree();
    REQUIRE(false == tree.getCellAlive(0, 0));
    REQUIRE(1 == tree.height());
  }

  SECTION("Creating a tree from empty points returns an empty quad") {
    auto points = vector<pair<int64_t, int64_t>>();
    QuadTree tree = QuadTree(points);

    REQUIRE(1 == tree.height());

    for (int y = tree.min; y < tree.max; y++) {
      for (int x = tree.min; x < tree.max; x++) {
        REQUIRE(false == tree.getCellAlive(x, y));
      }
    }
  }

  SECTION("Create a tree in the default middle should work as expected") {
    auto points = vector<pair<int64_t, int64_t>>{
        pair<int64_t, int64_t>(0, 0), pair<int64_t, int64_t>(-1, -1)};
    QuadTree tree = QuadTree(points);

    REQUIRE(1 == tree.height());
    for (int y = tree.min; y <= tree.max; y++) {
      for (int x = tree.min; x <= tree.max; x++) {
        if (x == y) {
          REQUIRE(true == tree.getCellAlive(x, y));
        } else {
          REQUIRE(false == tree.getCellAlive(x, y));
        }
      }
    }
  }
}

TEST_CASE("QuadTree cellSetting", "[QuadTree]") {
  SECTION("Setting a single cell tree to alive should report alive at (0,0)") {
    QuadTree tree = QuadTree{};
    tree.setCellAlive(0, 0);
    REQUIRE(true == tree.getCellAlive(0, 0));
  }

  SECTION("Setting (-1,-1) to alive on a new tree will report alive there, "
          "dead otherwise") {
    QuadTree tree = QuadTree{};
    tree.setCellAlive(-1, -1);
    REQUIRE(false == tree.getCellAlive(0, 0));
    REQUIRE(false == tree.getCellAlive(-1, 0));
    REQUIRE(false == tree.getCellAlive(0, -1));
    REQUIRE(true == tree.getCellAlive(-1, -1));
    REQUIRE(1 == tree.height());
  }

  SECTION("Setting (2,2) to alive on a new tree will report alive at (2,2), "
          "dead otherwise") {
    QuadTree tree = QuadTree{};
    tree.setCellAlive(2, 2);
    for (int y = tree.min; y <= tree.max; y++) {
      for (int x = tree.min; x <= tree.max; x++) {
        if (x == 2 && y == 2) {
          REQUIRE(true == tree.getCellAlive(x, y));
        } else {
          REQUIRE(false == tree.getCellAlive(x, y));
        }
      }
    }
    REQUIRE(3 == tree.height());
  }

  SECTION("Setting (-3,-3), (4,4) to alive on a new tree will report alive at "
          "there, dead otherwise") {
    QuadTree tree = QuadTree{};
    tree.setCellAlive(4, 4);
    tree.setCellAlive(-3, -3);

    for (int y = tree.min; y <= tree.max; y++) {
      for (int x = tree.min; x <= tree.max; x++) {
        if ((x == 4 && y == 4) || (x == -3 && y == -3)) {
          REQUIRE(true == tree.getCellAlive(x, y));
        } else {
          REQUIRE(false == tree.getCellAlive(x, y));
        }
      }
    }
    REQUIRE(4 == tree.height());
  }

  SECTION("Setting (-16,15) to alive on a new tree will report alive "
          "there, dead otherwise") {
    QuadTree tree = QuadTree{};
    tree.setCellAlive(-16, 15);

    for (int y = tree.min; y <= tree.max; y++) {
      for (int x = tree.min; x <= tree.max; x++) {
        if (x == -16 && y == 15) {
          REQUIRE(true == tree.getCellAlive(x, y));
        } else {
          REQUIRE(false == tree.getCellAlive(x, y));
        }
      }
    }
    REQUIRE(5 == tree.height());
  }

  SECTION("Setting (8,0) to alive on a new tree will report alive "
          "there, dead otherwise") {
    QuadTree tree = QuadTree{};
    tree.setCellAlive(8, 0);

    for (int y = tree.min; y <= tree.max; y++) {
      for (int x = tree.min; x <= tree.max; x++) {
        if (x == 8 && y == 0) {
          REQUIRE(true == tree.getCellAlive(x, y));
        } else {
          REQUIRE(false == tree.getCellAlive(x, y));
        }
      }
    }
    REQUIRE(5 == tree.height());
  }

  SECTION("Setting on a massive pattern should work as expected") {
    auto i0 = pair<int64_t, int64_t>(2, 1);
    auto i1 = pair<int64_t, int64_t>(1, 3);
    auto i2 = pair<int64_t, int64_t>(2, 3);
    auto i3 = pair<int64_t, int64_t>(4, 2);

    auto i4 = pair<int64_t, int64_t>(5, 3);
    auto i5 = pair<int64_t, int64_t>(6, 3);
    auto i6 = pair<int64_t, int64_t>(7, 3);

    auto i7 = pair<int64_t, int64_t>(20000000000, 1);
    auto i8 = pair<int64_t, int64_t>(20000000000, 0);
    auto i9 = pair<int64_t, int64_t>(20000000000, -1);

    auto points =
        vector<pair<int64_t, int64_t>>{i0, i1, i2, i3, i4, i5, i6, i7, i8, i9};

    QuadTree tree = QuadTree(points);

    REQUIRE(true == tree.getCellAlive(2, 1));
    REQUIRE(true == tree.getCellAlive(1, 3));
    REQUIRE(true == tree.getCellAlive(2, 3));
    REQUIRE(true == tree.getCellAlive(4, 2));
    REQUIRE(true == tree.getCellAlive(5, 3));
    REQUIRE(true == tree.getCellAlive(6, 3));
    REQUIRE(true == tree.getCellAlive(7, 3));
    REQUIRE(true == tree.getCellAlive(20000000000, 1));
    REQUIRE(true == tree.getCellAlive(20000000000, 0));
    REQUIRE(true == tree.getCellAlive(20000000000, -1));
  }
}

TEST_CASE("QuadTree growth", "[QuadTree]") {
  SECTION("A quad tree's root will have a height equal to the number of calls "
          "+ 1") {
    QuadTree tree = QuadTree{};

    for (int i = 0; i < 10; i++) {
      REQUIRE(i + 1 == tree.root->height);
      tree.growTree(1);
    }

    REQUIRE(11 == tree.height());
  }
}

TEST_CASE("QuadTree update", "[QuadTree]") {
  SECTION("Calling update on an empty root will produce an empty root.") {
    QuadTree tree = QuadTree{};

    tree.nextGeneration();

    for (int y = tree.min; y <= tree.max; y++) {
      for (int x = tree.min; x <= tree.max; x++) {
        REQUIRE(false == tree.getCellAlive(x, y));
      }
    }
  }

  SECTION("Calling update 100 on an empty root will always produce an "
          "empty root.") {
    QuadTree tree = QuadTree{};

    for (int i = 0; i < 100; i++) {
      tree.nextGeneration();

      for (int y = tree.min; y <= tree.max; y++) {
        for (int x = tree.min; x <= tree.max; x++) {
          REQUIRE(false == tree.getCellAlive(x, y));
        }
      }
    }
  }

  SECTION("Calling update on a grid with a single vertical pulsar will "
          "produce a horizontal pulsar.") {
    QuadTree tree = QuadTree{};

    tree.setCellAlive(0, -1);
    tree.setCellAlive(0, 0);
    tree.setCellAlive(0, 1);

    for (int y = tree.min; y <= tree.max; y++) {
      for (int x = tree.min; x <= tree.max; x++) {
        if (x == 0 && (y >= -1 && y <= 1)) {
          REQUIRE(true == tree.getCellAlive(x, y));
        } else {
          REQUIRE(false == tree.getCellAlive(x, y));
        }
      }
    }

    tree.nextGeneration();

    for (int y = tree.min; y <= tree.max; y++) {
      for (int x = tree.min; x <= tree.max; x++) {
        if (y == 0 && (x >= -1 && x <= 1)) {
          REQUIRE(true == tree.getCellAlive(x, y));
        } else {
          REQUIRE(false == tree.getCellAlive(x, y));
        }
      }
    }
  }

  SECTION("Calling update 100 times on a grid with a single vertical pulsar "
          "will fluctuate between the two pulsars.") {
    QuadTree tree = QuadTree{};

    tree.setCellAlive(0, -1);
    tree.setCellAlive(0, 0);
    tree.setCellAlive(0, 1);

    for (int i = 0; i < 100; i++) {
      for (int y = tree.min; y <= tree.max; y++) {
        for (int x = tree.min; x <= tree.max; x++) {
          if (x == 0 && (y >= -1 && y <= 1)) {
            REQUIRE(true == tree.getCellAlive(x, y));
          } else {
            REQUIRE(false == tree.getCellAlive(x, y));
          }
        }
      }

      tree.nextGeneration();

      for (int y = tree.min; y <= tree.max; y++) {
        for (int x = tree.min; x <= tree.max; x++) {
          if (y == 0 && (x >= -1 && x <= 1)) {
            REQUIRE(true == tree.getCellAlive(x, y));
          } else {
            REQUIRE(false == tree.getCellAlive(x, y));
          }
        }
      }

      tree.nextGeneration();
    }
  }

  SECTION("Calling update on a very far vertical pulsar should produce "
          " a very far horizontal pulsar") {
    int64_t big = INT64_MAX - 1;
    int64_t small = INT64_MIN + 1;
    auto i0 = pair<int64_t, int64_t>(big, -1);
    auto i1 = pair<int64_t, int64_t>(big, 0);
    auto i2 = pair<int64_t, int64_t>(big, 1);
    auto i3 = pair<int64_t, int64_t>(small, -1);
    auto i4 = pair<int64_t, int64_t>(small, 0);
    auto i5 = pair<int64_t, int64_t>(small, 1);

    auto points = vector<pair<int64_t, int64_t>>{i0, i1, i2, i3, i4, i5};

    QuadTree tree = QuadTree(points);
    unsigned int originalHeight = tree.root->height;

    REQUIRE(true == tree.getCellAlive(big, -1));
    REQUIRE(true == tree.getCellAlive(big, 0));
    REQUIRE(true == tree.getCellAlive(big, 1));
    REQUIRE(true == tree.getCellAlive(small, -1));
    REQUIRE(true == tree.getCellAlive(small, 0));
    REQUIRE(true == tree.getCellAlive(small, 1));

    tree.nextGeneration();

    REQUIRE(true == tree.getCellAlive(small - 1, 0));
    REQUIRE(true == tree.getCellAlive(small, 0));
    REQUIRE(true == tree.getCellAlive(small + 1, 0));
    REQUIRE(true == tree.getCellAlive(big - 1, 0));
    REQUIRE(true == tree.getCellAlive(big, 0));
    REQUIRE(true == tree.getCellAlive(big + 1, 0));
  }
}
