#include "table.hpp"

#include <sstream>

Table Table::intersection(const Table& other) {
  Table out;
  for (auto&& [key, value] : table_) {
    if (!other.table_.count(key)) {
      continue;
    }

    std::ostringstream joined;
    joined << value.name << ',' << other.table_.at(key).name;
    out.insert(key, joined.str());
  }
  return out;
}

Table Table::symmetric_difference(const Table& other) {
  Table out;
  Table instersection = this->intersection(other);
  for (auto&& [key, value] : table_) {
    if (instersection.table_.count(key)) {
      continue;
    }

    std::ostringstream joined;
    joined << value.name << ',';
    out.insert(key, joined.str());
  }

  for (auto&& [key, value] : other.table_) {
    if (instersection.table_.count(key)) {
      continue;
    }

    std::ostringstream joined;
    joined << ',' << value.name;
    out.insert(key, joined.str());
  }

  return out;
}

std::string Table::print() {
  std::ostringstream out;
  for (auto&& [key, value] : table_) {
    out << '\t' << value.id << ',' << value.name << '\n';
  }
  return out.str();
}