#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include <spdlog/spdlog.h>

#include "messages.hpp"
#include "multicast.hpp"


/**
 * Open the provide host file and read contents. Return host names in vector
 * of string.
 */
std::vector<std::string> parse_hostfile(const std::string& hostfile) {
  spdlog::info("Parsing host file");
  std::ifstream fhosts{hostfile.c_str(), std::ios_base::in};
  if (!fhosts.is_open()) {
    std::string err{"Unable to open file "};
    err.append(hostfile);
    throw std::runtime_error{err};
  }

  std::vector<std::string> hosts{};
  std::string host{};
  while (std::getline(fhosts, host)) {
    spdlog::info("Discovered host: {}", host);
    hosts.push_back(host);
  }

  fhosts.close();
  return hosts;
}

int main(int argc, char** argv) {
  using namespace boost::program_options;
  spdlog::set_level(spdlog::level::debug);

  bool is_help{};
  options_description description{"multicast "};
  description.add_options()("help,h", bool_switch(&is_help),
                            "display a help dialog")(
      "port,p", value<uint16_t>(),
      "port the process will be listening on for incoming message")(
      "hostfile,z", value<std::string>(),
      "hostfile is the path to a file that contains the list of hostnames")(
      "count,c", value<int>()->default_value(0),
      "number of message to multicast");

  command_line_parser parser{argc, argv};
  parser.options(description);

  variables_map vm;
  try {
    auto parsed_result = parser.run();
    store(parsed_result, vm);
    notify(vm);
  } catch (const std::exception& e) {
    spdlog::error("{}", e.what());
    return -1;
  }
  spdlog::info("Parsed commandline vars");

  if (is_help) {
    std::cout << description;
    return 0;
  }
  if (vm["port"].empty()) {
    spdlog::error("Must provide a port");
    return -1;
  }
  if (vm["hostfile"].empty()) {
    spdlog::error("Must provide a hostfile");
    return -1;
  }

  const auto port = vm["port"].as<uint16_t>();
  const auto& hostfile = vm["hostfile"].as<std::string>();
  const auto count = vm["count"].as<int>();

  std::vector<std::string> hosts{};
  try {
    hosts = parse_hostfile(hostfile);
  } catch (std::runtime_error& e) {
    spdlog::error("{}", e.what());
    return -1;
  }
  spdlog::info("Parsed hostfile");

  // Find process id based on index in hosts file
  std::string my_hostname = boost::asio::ip::host_name();
  uint32_t process_id{};
  for (std::string host : hosts) {
    if (!host.compare(my_hostname)) break;
    process_id++;
  }
  spdlog::info("Process id: {}", process_id);

  try {
    spdlog::info("Creating multicaster");
    multicast::Multicaster multicaster{hosts, port, process_id};
    spdlog::info("Starting main event loop");
    int i{};
    while (true) {
      multicaster.poll();
      if (i < count) {
        multicaster.multicast(i);
        i++;
      }
    }
  } catch (std::exception& e) {
    spdlog::error("{}", e.what());
  }

  return 0;
}
