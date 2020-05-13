#include "multicast.hpp"

#include <iostream>
#include <boost/bind/bind.hpp>

#include "messages.hpp"

using namespace multicast;
using boost::asio::ip::udp;

Multicaster::Multicaster(std::vector<std::string>& hosts, uint16_t port,
                         uint32_t process_id)
    : socket_{io_context_, udp::endpoint{udp::v4(), port}},
      hosts_{hosts},
      port_{std::to_string(port)},
      process_id_{process_id} {
  start_receive();
}

Multicaster::~Multicaster() {
  for (auto m : queue_) {
    delete m;
  }
}

void Multicaster::multicast(uint32_t data) {
  spdlog::info("Multicasting message");
  messages::DataMessage msg{process_id_, last_msg_id_++, data};
  std::vector<uint32_t> msg_vect{};
  msg.serialize(msg_vect);
  spdlog::info("Process {} prepared message {}", process_id_, msg.msg_id);

  send_multi(boost::asio::buffer((char*)&msg_vect.front(), 16));
}

void Multicaster::start_receive() {
  socket_.async_receive_from(
      boost::asio::buffer((char*)&recv_buffer_.front(), 20), remote_endpoint_,
      boost::bind(&Multicaster::handle_receive, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

void Multicaster::handle_receive(const boost::system::error_code& error,
                    std::size_t /*bytes_transferred*/) {
  if (error) {
    spdlog::error("Error reading from socket");
    return;
  }

  uint32_t msg_type = ntohl(recv_buffer_[0]);

  switch (msg_type) {
    case 1: {
      spdlog::info("Received Data Message");
      messages::DataMessage* M = new messages::DataMessage{recv_buffer_};
      queue_.push_back(M);
      // Update msg id if necessary to prevent repeating msg id
      last_msg_id_ = std::max(last_msg_id_, M->msg_id);
      spdlog::info("Added M to queue");

      messages::AckMessage A{M->sender, M->msg_id, last_seq_received_ + 1,
                              process_id_};
      std::vector<uint32_t> ack_vect{};
      A.serialize(ack_vect);
      send_single(boost::asio::buffer((char*)&ack_vect.front(), 20),
                  M->sender);
      spdlog::info("Sent ack message");

      break;
    }
    case 2: {
      spdlog::info("Received Ack Message");
      // Sender collects all acks from all hosts and calculate final_seq
      messages::AckMessage A{recv_buffer_};
      for (auto m : queue_) {
        if (m->msg_id == A.msg_id) {
          m->acks_received++;
          if (A.proposed_seq > m->final_seq) {
            m->final_seq = A.proposed_seq;
            m->final_seq_proposer = A.proposer;
          }

          if (m->acks_received == hosts_.size()) {
            spdlog::info("All acks received");
            spdlog::info("Multicasting final_seq {} proposed by {}",
                          m->final_seq, m->final_seq_proposer);
            messages::SeqMessage S{m->sender, m->msg_id, m->final_seq,
                                    process_id_};
            std::vector<uint32_t> msg_vect{};
            S.serialize(msg_vect);
            send_multi(boost::asio::buffer((char*)&msg_vect.front(), 20));
          }
          break;
        }
      }
      break;
    }
    case 3: {
      spdlog::info("Received Seq Message");
      // ??? We don't reorder the queue, should we
      messages::SeqMessage S{recv_buffer_};
      messages::DataMessage* m{};
      for (auto it = queue_.begin(); it != queue_.end(); /*left empty*/) {
        m = *it;
        if (m->deliverable && m->final_seq < S.final_seq) {
          spdlog::info("Delivering message with sequence {}", m->final_seq);
          // deliver messages with lower seq and deliverable
          std::cout << process_id_ << ": Processed message " << m->msg_id
                    << " with from sender " << m->sender << " with seq ("
                    << m->final_seq << ", " << m->final_seq_proposer << ")"
                    << std::endl;
          // Update iterator if we remove the message
          it = queue_.erase(it);

        } else if (m->msg_id == S.msg_id) {
          // mark as deliverable and set appropriate fields
          spdlog::info("Marking message deliverable");
          m->deliverable = true;
          m->final_seq = S.final_seq;
          m->final_seq_proposer = S.final_seq_proposer;
          it++;

        } else {
          // Iterator condition
          it++;
        }
      }
      // update last_seq_received
      last_seq_received_ = S.final_seq;
      spdlog::info("Updated last_seq_received to {}", last_seq_received_);
      break;
    }
    default: {
      spdlog::error("Unknown message type received");
    }
  }

  start_receive();
}

void Multicaster::send_single(boost::asio::mutable_buffer message, int hostnum) {
  udp::resolver resolver{io_context_};
  udp::endpoint receiver_endpoint =
      *resolver.resolve(udp::v4(), hosts_[hostnum], port_).begin();
  socket_.async_send_to(
      message, receiver_endpoint,
      boost::bind(&Multicaster::handle_send, this, message,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
  spdlog::info("Sent message to {}", hosts_[hostnum]);
}

void Multicaster::send_multi(boost::asio::mutable_buffer message) {
  udp::resolver resolver{io_context_};
  udp::endpoint receiver_endpoint{};
  for (auto host : hosts_) {
    receiver_endpoint = *resolver.resolve(udp::v4(), host, port_).begin();
    socket_.async_send_to(
        message, receiver_endpoint,
        boost::bind(&Multicaster::handle_send, this, message,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }
  spdlog::info("Message successfully multicast");
}