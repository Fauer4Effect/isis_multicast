#include "gtest/gtest.h"
#include <boost/asio.hpp>

#include "messages.hpp"

/************************************************
 *  Data Message Tests
 ***********************************************/
TEST(DataMessageTest, TestConstructor) {
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t data{0xdeadbeef};
  messages::DataMessage m{sender, msg_id, data};
  ASSERT_EQ(m.type, 1);
  ASSERT_EQ(m.sender, sender);
  ASSERT_EQ(m.msg_id, msg_id);
  ASSERT_EQ(m.data, data);
  ASSERT_FALSE(m.deliverable);
  ASSERT_EQ(m.final_seq, 0);
  ASSERT_EQ(m.acks_received, 0);
  ASSERT_EQ(m.final_seq_proposer, 0);
}

TEST(DataMessageTest, TestDeserialize) {
  uint32_t type{1};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t data{0xdeadbeef};
  std::vector<uint32_t> buf;
  buf.push_back(htonl(type));
  buf.push_back(htonl(sender));
  buf.push_back(htonl(msg_id));
  buf.push_back(htonl(data));

  messages::DataMessage m{buf};
  ASSERT_EQ(m.type, 1);
  ASSERT_EQ(m.sender, sender);
  ASSERT_EQ(m.msg_id, msg_id);
  ASSERT_EQ(m.data, data);
  ASSERT_FALSE(m.deliverable);
  ASSERT_EQ(m.final_seq, 0);
  ASSERT_EQ(m.acks_received, 0);
  ASSERT_EQ(m.final_seq_proposer, 0);
}

TEST(DataMessageTest, TestDeserializeShortBuf) {
  uint32_t type{1};
  uint32_t sender{10};
  uint32_t msg_id{25};
  std::vector<uint32_t> buf;
  buf.push_back(htonl(type));
  buf.push_back(htonl(sender));
  buf.push_back(htonl(msg_id));

  ASSERT_THROW(messages::DataMessage m{buf}, std::runtime_error);
}

// Expected behavior for long buffer is to ignore the extra data.
TEST(DataMessageTest, TestDeserializeLongBuf) {
  uint32_t type{1};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t data{0xdeadbeef};
  std::vector<uint32_t> buf;
  buf.push_back(htonl(type));
  buf.push_back(htonl(sender));
  buf.push_back(htonl(msg_id));
  buf.push_back(htonl(data));
  buf.push_back(0xffffffff);

  messages::DataMessage m{buf};
  ASSERT_EQ(m.type, 1);
  ASSERT_EQ(m.sender, sender);
  ASSERT_EQ(m.msg_id, msg_id);
  ASSERT_EQ(m.data, data);
  ASSERT_FALSE(m.deliverable);
  ASSERT_EQ(m.final_seq, 0);
  ASSERT_EQ(m.acks_received, 0);
  ASSERT_EQ(m.final_seq_proposer, 0);
}

TEST(DataMessageTest, TestSerialize) {
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t data{0xdeadbeef};
  messages::DataMessage m{sender, msg_id, data};

  std::vector<uint32_t> buf{};
  m.serialize(buf);

  ASSERT_EQ(ntohl(buf[0]), 1);
  ASSERT_EQ(ntohl(buf[1]), sender);
  ASSERT_EQ(ntohl(buf[2]), msg_id);
  ASSERT_EQ(ntohl(buf[3]), data);
}

TEST(DataMessageTest, TestSerializeNonEmptyBuf) {
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t data{0xdeadbeef};
  messages::DataMessage m{sender, msg_id, data};
  
  std::vector<uint32_t> buf{5};
  ASSERT_THROW(m.serialize(buf), std::runtime_error);
}


/************************************************
 *  Ack Message Tests
 ***********************************************/
TEST(AckMessageTest, TestConstructor) {
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t proposed_seq{0xdeadbeef};
  uint32_t proposer{420};
  messages::AckMessage m{sender, msg_id, proposed_seq, proposer};

  ASSERT_EQ(m.type, 2);
  ASSERT_EQ(m.sender, sender);
  ASSERT_EQ(m.msg_id, msg_id);
  ASSERT_EQ(m.proposed_seq, proposed_seq);
  ASSERT_EQ(m.proposer, proposer);
}

TEST(AckMessageTest, TestDeserialize) {
  uint32_t type{2};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t proposed_seq{0xdeadbeef};
  uint32_t proposer{420};
  std::vector<uint32_t> buf;
  buf.push_back(htonl(type));
  buf.push_back(htonl(sender));
  buf.push_back(htonl(msg_id));
  buf.push_back(htonl(proposed_seq));
  buf.push_back(htonl(proposer));

  messages::AckMessage m{buf};
  ASSERT_EQ(m.type, 2);
  ASSERT_EQ(m.sender, sender);
  ASSERT_EQ(m.msg_id, msg_id);
  ASSERT_EQ(m.proposed_seq, proposed_seq);
  ASSERT_EQ(m.proposer, proposer);
}

TEST(AckMessageTest, TestDeserializeShortBuf) {
  uint32_t type{2};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t proposed_seq{0xdeadbeef};
  std::vector<uint32_t> buf;
  buf.push_back(htonl(type));
  buf.push_back(htonl(sender));
  buf.push_back(htonl(msg_id));
  buf.push_back(htonl(proposed_seq));

  ASSERT_THROW(messages::AckMessage m{buf}, std::runtime_error);
}

// Expected behavior for long buffer is to ignore the extra data.
TEST(AckMessageTest, TestDeserializeLongBuf) {
  uint32_t type{2};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t proposed_seq{0xdeadbeef};
  uint32_t proposer{420};
  std::vector<uint32_t> buf;
  buf.push_back(htonl(type));
  buf.push_back(htonl(sender));
  buf.push_back(htonl(msg_id));
  buf.push_back(htonl(proposed_seq));
  buf.push_back(htonl(proposer));
  buf.push_back(htonl(0xffffffff));

  messages::AckMessage m{buf};
  ASSERT_EQ(m.type, 2);
  ASSERT_EQ(m.sender, sender);
  ASSERT_EQ(m.msg_id, msg_id);
  ASSERT_EQ(m.proposed_seq, proposed_seq);
  ASSERT_EQ(m.proposer, proposer);
}

TEST(AckMessageTest, TestSerialize) {
  uint32_t type{2};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t proposed_seq{0xdeadbeef};
  uint32_t proposer{420};
  messages::AckMessage m{sender, msg_id, proposed_seq, proposer};

  std::vector<uint32_t> buf{};
  m.serialize(buf);

  ASSERT_EQ(ntohl(buf[0]), 2);
  ASSERT_EQ(ntohl(buf[1]), sender);
  ASSERT_EQ(ntohl(buf[2]), msg_id);
  ASSERT_EQ(ntohl(buf[3]), proposed_seq);
  ASSERT_EQ(ntohl(buf[4]), proposer);
}

TEST(AckMessageTest, TestSerializeNonEmptyBuf) {
  uint32_t type{2};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t proposed_seq{0xdeadbeef};
  uint32_t proposer{420};
  messages::AckMessage m{sender, msg_id, proposed_seq, proposer};

  std::vector<uint32_t> buf{5};
  ASSERT_THROW(m.serialize(buf), std::runtime_error);
}


/************************************************
 *  Seq Message Tests
 ***********************************************/
TEST(SeqMessageTest, TestConstructor) {
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t final_seq{0xdeadbeef};
  uint32_t final_seq_proposer{420};
  messages::SeqMessage m{sender, msg_id, final_seq, final_seq_proposer};

  ASSERT_EQ(m.type, 3);
  ASSERT_EQ(m.sender, sender);
  ASSERT_EQ(m.msg_id, msg_id);
  ASSERT_EQ(m.final_seq, final_seq);
  ASSERT_EQ(m.final_seq_proposer, final_seq_proposer);
}

TEST(SeqMessageTest, TestDeserialize) {
  uint32_t type{3};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t final_seq{0xdeadbeef};
  uint32_t final_seq_proposer{420};
  std::vector<uint32_t> buf;
  buf.push_back(htonl(type));
  buf.push_back(htonl(sender));
  buf.push_back(htonl(msg_id));
  buf.push_back(htonl(final_seq));
  buf.push_back(htonl(final_seq_proposer));

  messages::SeqMessage m{buf};
  ASSERT_EQ(m.type, 3);
  ASSERT_EQ(m.sender, sender);
  ASSERT_EQ(m.msg_id, msg_id);
  ASSERT_EQ(m.final_seq, final_seq);
  ASSERT_EQ(m.final_seq_proposer, final_seq_proposer);
}

TEST(SeqMessageTest, TestDeserializeShortBuf) {
  uint32_t type{3};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t final_seq{0xdeadbeef};
  std::vector<uint32_t> buf;
  buf.push_back(htonl(type));
  buf.push_back(htonl(sender));
  buf.push_back(htonl(msg_id));
  buf.push_back(htonl(final_seq));

  ASSERT_THROW(messages::SeqMessage m{buf}, std::runtime_error);
}

// Expected behavior for long buffer is to ignore the extra data.
TEST(SeqMessageTest, TestDeserializeLongBuf) {
  uint32_t type{3};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t final_seq{0xdeadbeef};
  uint32_t final_seq_proposer{420};
  std::vector<uint32_t> buf;
  buf.push_back(htonl(type));
  buf.push_back(htonl(sender));
  buf.push_back(htonl(msg_id));
  buf.push_back(htonl(final_seq));
  buf.push_back(htonl(final_seq_proposer));
  buf.push_back(htonl(0xffffffff));

  messages::SeqMessage m{buf};
  ASSERT_EQ(m.type, 3);
  ASSERT_EQ(m.sender, sender);
  ASSERT_EQ(m.msg_id, msg_id);
  ASSERT_EQ(m.final_seq, final_seq);
  ASSERT_EQ(m.final_seq_proposer, final_seq_proposer);
}

TEST(SeqMessageTest, TestSerialize) {
  uint32_t type{3};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t final_seq{0xdeadbeef};
  uint32_t final_seq_proposer{420};
  messages::SeqMessage m{sender, msg_id, final_seq, final_seq_proposer};

  std::vector<uint32_t> buf{};
  m.serialize(buf);

  ASSERT_EQ(ntohl(buf[0]), 3);
  ASSERT_EQ(ntohl(buf[1]), sender);
  ASSERT_EQ(ntohl(buf[2]), msg_id);
  ASSERT_EQ(ntohl(buf[3]), final_seq);
  ASSERT_EQ(ntohl(buf[4]), final_seq_proposer);
}

TEST(SeqMessageTest, TestSerializeNonEmptyBuf) {
  uint32_t type{3};
  uint32_t sender{10};
  uint32_t msg_id{25};
  uint32_t final_seq{0xdeadbeef};
  uint32_t final_seq_proposer{420};
  messages::AckMessage m{sender, msg_id, final_seq, final_seq_proposer};

  std::vector<uint32_t> buf{5};
  ASSERT_THROW(m.serialize(buf), std::runtime_error);
}