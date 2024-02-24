#pragma once

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <string>

class IPrintable {
 public:
  virtual void write(const std::string &) = 0;
  virtual ~IPrintable() {}
};

class PrintComposite : public IPrintable {
 protected:
  std::list<std::shared_ptr<IPrintable>> children_;

 public:
  ~PrintComposite() = default;

  void Add(std::shared_ptr<IPrintable> component) {
    children_.emplace_back(std::move(component));
  }

  void Remove(std::shared_ptr<IPrintable> component) {
    children_.remove(component);
  }

  void write(const std::string &data) final {
    for (auto &&c : children_) {
      c->write(data);
    }
  }
};

class ConsolePrint : public IPrintable {
 public:
  void write(const std::string &data) final { std::cout << data; };
};

class FilePrint : public IPrintable {
 public:
  void write(const std::string &data) final {
    using namespace std::chrono;
    std::string fileName = "server.log";
    std::ofstream file(fileName);
    file << data;
    file.close();
  };
};
