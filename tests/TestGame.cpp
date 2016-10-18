#include "../Game.hpp"
#include "catch.hpp"
#include <cstdint>

TEST_CASE("Game clampMin", "[Game]") {
  SECTION("value + offset far away from INT64_MIN will return value - "
          "offset - 1") {
    int64_t y = 0;
    unsigned int height = 800;
    REQUIRE(y - height - 1 == Game::clampMin(y, height, 1));
  }

  SECTION("value + offset close to INT64_MIN will return value - "
          "offset") {
    int64_t y = INT64_MIN + 800;
    unsigned int height = 800;
    REQUIRE(y - height == Game::clampMin(y, height, 1));
  }

  SECTION("value closer than offset to INT64_MIN will return INT64_MIN") {
    int64_t y = INT64_MIN + 200;
    unsigned int height = 800;
    REQUIRE(INT64_MIN == Game::clampMin(y, height, 1));
  }
}

TEST_CASE("Game clampMax", "[Game]") {
  SECTION("value - offset far away from INT64_MAX will return value + "
          "offset + 1") {
    int64_t y = 0;
    unsigned int height = 800;
    REQUIRE(y + height + 1 == Game::clampMax(y, height, 1));
  }

  SECTION("value - offset close to INT64_MAX will return value - "
          "offset") {
    int64_t y = INT64_MAX - 800;
    unsigned int height = 800;
    REQUIRE(y + height == Game::clampMax(y, height, 1));
  }

  SECTION("value closer than offset to INT64_MAX will return INT64_MAX") {
    int64_t y = INT64_MAX - 200;
    unsigned int height = 800;
    REQUIRE(INT64_MAX == Game::clampMax(y, height, 1));
  }
}
