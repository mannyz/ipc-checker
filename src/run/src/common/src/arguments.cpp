#include "run/as/common/arguments.hpp"

#include <cassert>

Arguments::Arguments(std::initializer_list<Item> items) {
  for (auto i : items) {
    if (i.required) {
      ++_requred_items_count;
    }
    _items.emplace(i.key, std::move(i));
  }
}

bool Arguments::HasNotParsedRequiredArguments() const {
  return _requred_items_count;
}

void Arguments::ParseArgument(int key, std::string_view value) const {
  auto it = _items.find(key);
  assert(it != std::end(_items));
  it->second.parser(value);
  if (it->second.required && !it->second.parsed) {
    --_requred_items_count;
  }
  it->second.parsed = true;
}