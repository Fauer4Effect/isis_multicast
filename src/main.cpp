#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "messages.hpp"

#include "spdlog/spdlog.h"

using DataMessage = struct DataMessage;
using AckMessage = struct AckMessage;
using SeqMessage = struct SeqMessage;

// Sender multicasts the message to everyone
// Upon receiving a message M each receiver Ri
// 1. Adds M to the queue
// 2. Marks the message undeliverable
// 3. Sends ack to the sender with a sequence number seq that is the latest
// sequence number received + 1, suffixed with the Ri
// ’s ID.
// Sender collects all acks from the receivers
// 1. calculates final_seq = maximum ({seqi}
// )
// 2. multicasts final_seq to all processes
// Upon receiving final seq each receiver
// 1. marks M as deliverable,
// 2. reorders the queue based on seq
// 3. delivers the set of messages with lower seq and marked as deliverable. 

int main(int argc, char** argv) {
  using namespace boost::program_options;
  spdlog::set_level(spdlog::level::debug);
  
  bool is_help{};
  options_description description{ "multicast " };
  description.add_options()
    ("help,h", bool_switch(&is_help), "display a help dialog")
    ("port,p", value<int>(), 
      "port the process will be linstening on for incoming message")
    ("hostfile,z", value<std::string>(),
      "hostfile is the path to a file that contains the list of hostnames")
    ("count,c", value<int>()->default_value(0), "number of message to multicast");

  command_line_parser parser{ argc, argv };
  parser.options(description);
  
  variables_map vm;
  try {
    auto parsed_result = parser.run();
    store(parsed_result, vm);
    notify(vm);
    
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return -1;
  }

  if (is_help) {
    std::cout << description;
    return 0;
  }
  if (vm["port"].empty()) {
    std::cerr << "You must provide a port.\n";
    return -1;
  }
  if (vm["hostfile"].empty()) {
    std::cerr << "You must provide a hostfile.\n";
    return -1;
  }
  
  const auto port = vm["port"].as<int>();
  const auto& hostfile = vm["hostfile"].as<std::string>();
  const auto count = vm["count"].as<int>();
}
