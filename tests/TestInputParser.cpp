#include "../InputParser.hpp"
#include "catch.hpp"

TEST_CASE("InputParser strToInt64", "[InputParser]") {
  SECTION("strToInt64('42') should return 42") {
    char const *fourtyTwoChar = {"42"};

    int64_t fourtyTwoInt = InputParser::strToInt64(fourtyTwoChar);
    REQUIRE(42 == fourtyTwoInt);
  }

  SECTION("strToInt64(' 42') should return 42") {
    char const *fourtyTwoChar = {" 42"};

    int64_t fourtyTwoInt = InputParser::strToInt64(fourtyTwoChar);
    REQUIRE(42 == fourtyTwoInt);
  }

  SECTION("strToInt64('42 ') should return 42") {
    char const *fourtyTwoChar = {"42 "};

    int64_t fourtyTwoInt = InputParser::strToInt64(fourtyTwoChar);
    REQUIRE(42 == fourtyTwoInt);
  }

  SECTION("strToInt64('42\\t') should return 42") {
    char const *fourtyTwoChar = {"42\t"};

    int64_t fourtyTwoInt = InputParser::strToInt64(fourtyTwoChar);
    REQUIRE(42 == fourtyTwoInt);
  }

  SECTION("strToInt64('42\\n') should return 42") {
    char const *fourtyTwoChar = {"42\n"};

    int64_t fourtyTwoInt = InputParser::strToInt64(fourtyTwoChar);
    REQUIRE(42 == fourtyTwoInt);
  }

  SECTION("strToInt64(' 42 ') should return 42") {
    char const *fourtyTwoChar = {" 42 "};

    int64_t fourtyTwoInt = InputParser::strToInt64(fourtyTwoChar);
    REQUIRE(42 == fourtyTwoInt);
  }

  SECTION("strToInt64('(42)') should return 42") {
    char const *fourtyTwoChar = {"(42)"};

    int64_t fourtyTwoInt = InputParser::strToInt64(fourtyTwoChar);
    REQUIRE(42 == fourtyTwoInt);
  }

  SECTION("strToInt64('(42,') should return 42") {
    char const *fourtyTwoChar = {"(42,"};

    int64_t fourtyTwoInt = InputParser::strToInt64(fourtyTwoChar);
    REQUIRE(42 == fourtyTwoInt);
  }

  SECTION("strToInt64(', 42') should return 42") {
    char const *fourtyTwoChar = {"(42,"};

    int64_t fourtyTwoInt = InputParser::strToInt64(fourtyTwoChar);
    REQUIRE(42 == fourtyTwoInt);
  }

  SECTION(
      "strToInt64('9223372036854775807) should return 9223372036854775807") {
    char const *maxChar = {"9223372036854775807"};

    int64_t maxInt = InputParser::strToInt64(maxChar);
    REQUIRE(INT64_MAX == maxInt);
  }

  SECTION(
      "strToInt64('-9223372036854775808) should return 9223372036854775807") {
    char const *minChar = {"-9223372036854775808"};

    int64_t minInt = InputParser::strToInt64(minChar);
    REQUIRE(INT64_MIN == minInt);
  }

  SECTION("strToInt64('0') should return 0") {
    char const *zeroChar = {"0"};

    int64_t zeroInt = InputParser::strToInt64(zeroChar);
    REQUIRE(0 == zeroInt);
  }

  SECTION("strToInt64('-0') should return 0") {
    char const *zeroChar = {"-0"};

    int64_t zeroInt = InputParser::strToInt64(zeroChar);
    REQUIRE(0 == zeroInt);
  }
}
