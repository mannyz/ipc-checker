#pragma once

#include <string>
#include <vector>

class SimpleRoundGenerator {
public:
  SimpleRoundGenerator(std::size_t chunks_size, std::size_t chunks_total_count);

public:
  void ToFirst();
  bool HasNext() const;
  std::string Next();

private:
  std::vector<std::string> _msgs;
  std::vector<std::string>::const_iterator _it_msgs;
};
