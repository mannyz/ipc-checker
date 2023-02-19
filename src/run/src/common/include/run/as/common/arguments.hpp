#pragma once

#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>

class Arguments {
  struct Item {
    std::string name;
    const int key;
    const bool required = false;
    const std::function<void(std::string_view)> parser;
    bool parsed = false;
  };

public:
  static Item Required(std::string name, const int key,
                       const std::function<void(std::string_view)> parser) {
    return Item{name, key, true, parser};
  }

  static Item Optional(std::string name, const int key,
                       const std::function<void(std::string_view)> parser) {
    return Item{name, key, false, parser};
  }

public:
  Arguments(std::initializer_list<Item> items);

public:
  bool HasNotParsedRequiredArguments() const;
  void ParseArgument(int key, std::string_view value) const;

private:
  mutable int _requred_items_count = 0;
  mutable std::unordered_map<int, Item> _items;
};
