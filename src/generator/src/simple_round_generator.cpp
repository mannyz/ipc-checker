#include "generator/simple_round_generator.hpp"

#include <algorithm>

namespace {

static constexpr char kAlphabet[] = "abcdefghijklmnopqrstuvwxyz1234567890";
static constexpr std::size_t kAlphabetSize =
    sizeof(kAlphabet) / sizeof(char) - 1;

} // namespace

SimpleRoundGenerator::SimpleRoundGenerator(std::size_t chunks_size,
                                           std::size_t chunks_total_count) {
  _msgs.reserve(chunks_total_count);

  std::size_t alphabet_global_idx = 0;
  std::size_t alphabet_size =
      (chunks_size % kAlphabetSize) ? kAlphabetSize : kAlphabetSize - 1;
  for (std::size_t i = chunks_total_count; i != 0; --i) {
    std::string msg;
    msg.reserve(chunks_size);

    std::size_t len = chunks_size;
    while (len) {
      auto alphabet_idx = alphabet_global_idx % alphabet_size;
      auto count = alphabet_size - alphabet_idx;
      count = std::min(count, len);
      msg += std::string_view{&kAlphabet[alphabet_idx], count};

      len -= count;
      alphabet_global_idx += count;
    }

    _msgs.emplace_back(std::move(msg));
  }
}

void SimpleRoundGenerator::ToFirst() { _it_msgs = std::cbegin(_msgs); }

std::string SimpleRoundGenerator::Next() { return *_it_msgs++; }

bool SimpleRoundGenerator::HasNext() const {
  return _it_msgs != std::cend(_msgs);
}
