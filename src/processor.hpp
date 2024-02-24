#pragma once

#include <sstream>

#include "print.hpp"
#include "table.hpp"

class CommandProcessor {
 public:
  explicit CommandProcessor(std::shared_ptr<IPrintable> printer)
      : print_(printer) {}

  void push(const std::string &line) {
    if (!line.size()) {
      return;
    }

    std::istringstream stream(line);
    std::string parsed[max_args_];
    std::string word;
    size_t iter = 0;
    while (getline(stream, word, ' ') && iter < max_args_) {
      parsed[iter++] = word;
    }

    Commands cmd = Commands::SIZE;
    for (size_t it = 0; it < static_cast<size_t>(Commands::SIZE); ++it) {
      if (parsed[0].compare(CommandAliases[it]) != 0) {
        continue;
      }

      cmd = static_cast<Commands>(it);
      break;
    }

    bool ok = true;
    std::string msg, res;
    switch (cmd) {
      case Commands::INSERT: {
        int id = std::atoi(parsed[2].c_str());
        if (!memory_[parsed[1]].insert(id, parsed[3])) {
          ok = false;
          msg = "duplicate key";
        }
      } break;
      case Commands::TRUNCATE: {
        memory_[parsed[1]].truncate();
      } break;
      case Commands::INTERSECTION: {
        if (memory_.size() < 2) {
          ok = false;
          msg = "not enough tables were created";
        } else {
          decltype(memory_)::iterator it = memory_.begin();
          Table result = (it++)->second;
          for (; it != memory_.end(); it++) {
            result = it->second.intersection(result);
          }
          res = result.print();
        }
      } break;
      case Commands::SYMMETRIC_DIFFERENCE: {
        if (memory_.size() < 2) {
          ok = false;
          msg = "not enough tables were created";
        } else {
          decltype(memory_)::iterator it = memory_.begin();
          Table result = (it++)->second;
          for (; it != memory_.end(); it++) {
            result = it->second.symmetric_difference(result);
          }
          res = result.print();
        }
      } break;
      case Commands::SIZE:
      default:
        ok = false;
        msg = "unknown command";
        break;
    }

    const auto status = ok ? "OK " : "ERR ";
    print_->write(res + status + msg + '\n');
  }

 private:
  std::map<std::string, Table> memory_;
  std::shared_ptr<IPrintable> print_;

  enum class Commands : unsigned {
    INSERT,
    TRUNCATE,
    INTERSECTION,
    SYMMETRIC_DIFFERENCE,
    SIZE,
  };

  static const size_t commands_amount_ = static_cast<size_t>(Commands::SIZE);
  static const size_t max_args_ = 4;
  const char *CommandAliases[commands_amount_] = {
      "INSERT",                // [Commands::INSERT] =
      "TRUNCATE",              // [Commands::TRUNCATE] =
      "INTERSECTION",          // [Commands::INTERSECTION] =
      "SYMMETRIC_DIFFERENCE",  // [Commands::SYMMETRIC_DIFFERENCE] =
  };
};
