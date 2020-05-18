#pragma once
#include <cstdint>
#include <vector>
#include <boost/asio.hpp>

#include <spdlog/spdlog.h>

#include "messages.hpp"

namespace multicast {

class Multicaster {
 public:
  /**
   * Constructor and being listening for connections.
   */
  Multicaster(std::vector<std::string>& hosts, uint16_t port,
              uint32_t process_id);
  ~Multicaster();

  /**
   * Construct a DataMessage with the provided data and multicast the message
   * to all known hosts.
   */
  void multicast(uint32_t data);

  /**
   * Check for activity and run ready handlers.
   */
  void poll() { io_context_.poll(); }

 private:
  /**
   * Receives a connection on the socket and reads the received message into
   * recv_buffer_. Passes responsibility to handle_receive() to handle the
   * message.
   */
  void start_receive();

  /**
   * Determine the type of the received message and respond accordingly.
   * DataMessage:
   *  Mark message undeliverable.
   *  Add message to queue.
   *  Send Ack with proposed sequence number of last received seq + 1.
   * AckMessage:
   *  Collect acks from all hosts in hostsfile.
   *  Determine final sequence number by max(proposed sequence).
   *  Once all acks received send Seq to all hosts.
   * SeqMessage:
   *  Mark message as deliverable and set final sequence number.
   *  Go thorugh queue and deliver all messages that are deliverable with final
   *    sequence number less than M's sequence.
   */
  void handle_receive(const boost::system::error_code& error,
                      std::size_t /*bytes_transferred*/);

  /**
   * Send buffer to host indexed with hostnum in hostsfile.
   */
  void send_single(boost::asio::mutable_buffer message, int hostnum);

  /**
   * Send buffer to all hosts.
   */
  void send_multi(boost::asio::mutable_buffer message);

  /**
   * Dummy handle for async sending of message.
   */
  void handle_send(boost::asio::mutable_buffer /*message*/,
                   const boost::system::error_code& /*error*/,
                   std::size_t /*bytes_transferred*/) {}

  boost::asio::io_context io_context_{};
  boost::asio::ip::udp::socket socket_;
  std::string port_;
  std::vector<std::string> hosts_;
  boost::asio::ip::udp::endpoint remote_endpoint_;
  std::vector<uint32_t> recv_buffer_{5};
  std::vector<messages::DataMessage*> queue_{};
  uint32_t last_seq_received_{};
  uint32_t process_id_;
  uint32_t last_msg_id_{};
};
} // namespace multicast