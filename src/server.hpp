#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include "print.hpp"
#include "processor.hpp"
#include "project.h"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class JoinContext {
 public:
  using client = std::shared_ptr<IPrintable>;

  void join(client connection) {
    clients_.insert(connection);
    struct ProjectInfo info = {};
    connection->write(std::string{} + "Welcome to the " + info.nameString +
                      ": " + info.versionString + "\n");
  }
  void leave(client connection) {
    connection->write("Disconnected from a Join server\n");
    clients_.erase(connection);
  }

  void write(const std::string &data) {
    for (auto client : clients_) client->write(data);
  }

 private:
  std::set<client> clients_;
};

class JoinSession : public IPrintable,
                    public std::enable_shared_from_this<JoinSession> {
 public:
  JoinSession(tcp::socket socket, JoinContext &context,
              std::shared_ptr<CommandProcessor> proc,
              std::shared_ptr<PrintComposite> printer)
      : socket_(std::move(socket)),
        timer_(socket_.get_executor()),
        context_(context),
        proc_(proc),
        print_(printer) {
    timer_.expires_at(std::chrono::steady_clock::time_point::max());
  }

  void start() {
    context_.join(shared_from_this());
    print_->Add(shared_from_this());

    asio::co_spawn(
        socket_.get_executor(),
        [self = shared_from_this()] { return self->Inbound(); },
        asio::detached);

    asio::co_spawn(
        socket_.get_executor(),
        [self = shared_from_this()] { return self->Outbound(); },
        asio::detached);
  }

  void write(const std::string &data) {
    write_msgs_.push_back(data);
    timer_.cancel_one();
  }

 private:
  asio::awaitable<void> Inbound() {
    try {
      for (std::string read_msg;;) {
        std::size_t n = co_await asio::async_read_until(
            socket_, asio::dynamic_buffer(read_msg, 1024), "\n",
            asio::use_awaitable);

        proc_->push(read_msg.substr(0, n - 1));
        read_msg.erase(0, n);
      }
    } catch (std::exception &) {
      stop();
    }
  }

  asio::awaitable<void> Outbound() {
    try {
      while (socket_.is_open()) {
        if (write_msgs_.empty()) {
          boost::system::error_code ec;
          co_await timer_.async_wait(redirect_error(asio::use_awaitable, ec));
        } else {
          co_await asio::async_write(socket_, asio::buffer("> " + write_msgs_.front() + "< "),
                                     asio::use_awaitable);

          write_msgs_.pop_front();
        }
      }
    } catch (std::exception &) {
      stop();
    }
  }

  void stop() {
    context_.leave(shared_from_this());
    print_->Remove(shared_from_this());
    socket_.close();
    timer_.cancel();
  }

  tcp::socket socket_;
  asio::steady_timer timer_;
  JoinContext &context_;
  std::deque<std::string> write_msgs_;
  std::shared_ptr<CommandProcessor> proc_;
  std::shared_ptr<PrintComposite> print_;
};
