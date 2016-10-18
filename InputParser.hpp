#ifndef INPUTPARSER_HPP
#define INPUTPARSER_HPP
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

class InputParser {
public:
  static std::vector<std::pair<int64_t, int64_t>> getPoints(int, char *[]);
  static int64_t strToInt64(std::string);
};

#endif // INPUTPARSER_HPP
