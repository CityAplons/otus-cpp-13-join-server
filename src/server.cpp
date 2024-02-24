#include "server.hpp"

#include <boost/program_options.hpp>

asio::awaitable<void> Listener(uint16_t port) {
  JoinContext context;
  const auto executor = co_await asio::this_coro::executor;
  auto printer = std::make_shared<PrintComposite>();
  printer->Add(std::make_shared<ConsolePrint>());

  tcp::acceptor acceptor{executor, {tcp::v4(), port}};
  for (;;) {
    auto socket = co_await acceptor.async_accept(asio::use_awaitable);
    std::make_shared<JoinSession>(
        std::move(socket), context,
        std::make_shared<CommandProcessor>(printer), printer)
        ->start();
  }
}

void Run(uint16_t port) {
  asio::io_context ctx;

  asio::signal_set signals{ctx, SIGINT, SIGTERM};
  signals.async_wait([&](auto, auto) { ctx.stop(); });

  asio::co_spawn(ctx, Listener(port), asio::detached);

  ctx.run();
}

int main(int argc, char const *argv[]) {
  uint16_t port = 9000;
  namespace po = boost::program_options;
  po::options_description desc("Join asio server");
  desc.add_options()("help", "Produce this help message")(
      "port,p", po::value<uint16_t>(), "TCP Port");

  try {
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      desc.print(std::cout);
      return 1;
    }

    if (vm.count("port")) {
      port = vm["port"].as<uint16_t>();
    }

  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
    desc.print(std::cout);
  }

  try {
    Run(port);
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
