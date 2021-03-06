#include "messages.hpp"

#include <boost/asio.hpp>

using namespace messages;

DataMessage::DataMessage(uint32_t sender, uint32_t msg_id, uint32_t data)
    : type{1},
      sender{sender},
      msg_id{msg_id},
      data{data},
      deliverable{false},
      final_seq{},
      acks_received{},
      final_seq_proposer{} {}

DataMessage::DataMessage(std::vector<uint32_t>& buf)
    : type{ntohl(buf[0])},
      sender{ntohl(buf[1])},
      msg_id{ntohl(buf[2])},
      data{ntohl(buf[3])},
      deliverable{false},
      final_seq{},
      acks_received{},
      final_seq_proposer{} {
  if (buf.size() < 4) {
    throw std::runtime_error("Attempted to deserialize from short buf");
  }
}

void DataMessage::serialize(std::vector<uint32_t>& buf) {
  if (!buf.empty()) {
    throw std::runtime_error("Attempted to serialize with non empty buf");
  }
  buf.push_back(htonl(this->type));
  buf.push_back(htonl(this->sender));
  buf.push_back(htonl(this->msg_id));
  buf.push_back(htonl(this->data));
}

AckMessage::AckMessage(uint32_t sender, uint32_t msg_id, uint32_t proposed_seq,
                       uint32_t proposer)
    : type{2},
      sender{sender},
      msg_id{msg_id},
      proposed_seq{proposed_seq},
      proposer{proposer} {}

AckMessage::AckMessage(std::vector<uint32_t>& buf)
    : type{ntohl(buf[0])},
      sender{ntohl(buf[1])},
      msg_id{ntohl(buf[2])},
      proposed_seq{ntohl(buf[3])},
      proposer{ntohl(buf[4])} {
  if (buf.size() < 5) {
    throw std::runtime_error("Attempted to deserialize from short buf");
  }
}

void AckMessage::serialize(std::vector<uint32_t>& buf) {
  if (!buf.empty()) {
    throw std::runtime_error("Attempted to serialize with non empty buf");
  }
  buf.push_back(htonl(this->type));
  buf.push_back(htonl(this->sender));
  buf.push_back(htonl(this->msg_id));
  buf.push_back(htonl(this->proposed_seq));
  buf.push_back(htonl(this->proposer));
}

SeqMessage::SeqMessage(uint32_t sender, uint32_t msg_id, uint32_t final_seq,
                       uint32_t final_seq_proposer)
    : type{3},
      sender{sender},
      msg_id{msg_id},
      final_seq{final_seq},
      final_seq_proposer{final_seq_proposer} {}

SeqMessage::SeqMessage(std::vector<uint32_t>& buf)
    : type{ntohl(buf[0])},
      sender{ntohl(buf[1])},
      msg_id{ntohl(buf[2])},
      final_seq{ntohl(buf[3])},
      final_seq_proposer{ntohl(buf[4])} {
  if (buf.size() < 5) {
    throw std::runtime_error("Attempted to deserialize from short buf");
  }
}

void SeqMessage::serialize(std::vector<uint32_t>& buf) {
  if (!buf.empty()) {
    throw std::runtime_error("Attempted to serialize with non empty buf");
  }
  buf.push_back(htonl(this->type));
  buf.push_back(htonl(this->sender));
  buf.push_back(htonl(this->msg_id));
  buf.push_back(htonl(this->final_seq));
  buf.push_back(htonl(this->final_seq_proposer));
}