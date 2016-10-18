#include "InputParser.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <fstream>
#include <iostream>

/**
 * Given command-line input, return the points either from a life file, or the
 * raw points entered.
 * TODO: handle errors better, validate input, etc.
 */
std::vector<std::pair<int64_t, int64_t>> InputParser::getPoints(int argc,
                                                                char *argv[]) {
  if (argc == 3) {
    // check if we have a file flag, and if so read the file and parse the
    // points
    std::string flag = std::string(argv[1]);
    boost::trim(flag);
    if (flag == "-f") {
      std::ifstream ifs(argv[2]);
      std::string content((std::istreambuf_iterator<char>(ifs)),
                          (std::istreambuf_iterator<char>()));

      std::vector<std::string> tokens;
      boost::trim(content);
      boost::split(tokens, content, boost::is_any_of("\t\r\n "),
                   boost::token_compress_on);

      if (tokens.size() % 2 != 0) {
        throw "Invalid amount of points.";
      }

      auto points = std::vector<std::pair<int64_t, int64_t>>();

      for (int i = 0; i < tokens.size(); i += 2) {
        auto x = strToInt64(tokens[i]);
        auto y = strToInt64(tokens[i + 1]);
        points.push_back(std::pair<int64_t, int64_t>(x, y));
      }

      return points;
    }
  }

  // If we don't have a life file to read, instead read the raw points
  if (argc % 2 == 0) {
    throw "Invalid amount of points.";
  }

  auto points = std::vector<std::pair<int64_t, int64_t>>();
  for (int i = 1; i < argc; i += 2) {
    auto x = strToInt64(std::string(argv[i]));
    auto y = strToInt64(std::string(argv[i + 1]));
    points.push_back(std::pair<int64_t, int64_t>(x, y));
  }

  return points;
}

/**
 * Given a string, remove legal characters and convert it to an int64_t
 */
int64_t InputParser::strToInt64(std::string str) {
  boost::remove_erase_if(str, boost::is_any_of("(),"));
  boost::trim(str);

  char *end;
  return strtoll(str.c_str(), &end, 10);
}
