#include <catch2/catch_test_macros.hpp>
#include "nr_bb/fapi.hpp"

TEST_CASE("FAPI state machine happy path", "[fapi]") {
  nr_bb::FapiSession s;
  REQUIRE(s.state() == nr_bb::FapiState::Idle);
  auto r1 = s.handle({.type = nr_bb::FapiMsgType::CONFIG_REQ});
  REQUIRE(r1.type == nr_bb::FapiMsgType::CONFIG_RESP);
  REQUIRE(s.state() == nr_bb::FapiState::Configured);
  auto r2 = s.handle({.type = nr_bb::FapiMsgType::START_REQ});
  REQUIRE(r2.type == nr_bb::FapiMsgType::SLOT_IND);
  REQUIRE(s.state() == nr_bb::FapiState::Running);
  nr_bb::FapiMessage tx{.type = nr_bb::FapiMsgType::TX_DATA_REQ, .payload = {1, 0, 1}};
  auto r3 = s.handle(tx);
  REQUIRE(r3.type == nr_bb::FapiMsgType::CRC_IND);
  REQUIRE(r3.crc_ok);
}

TEST_CASE("FAPI sequencing error", "[fapi]") {
  nr_bb::FapiSession s;
  auto r = s.handle({.type = nr_bb::FapiMsgType::TX_DATA_REQ});
  REQUIRE(r.type == nr_bb::FapiMsgType::ERROR_IND);
  REQUIRE(s.state() == nr_bb::FapiState::Error);
}

TEST_CASE("FAPI JSON fixtures", "[fapi]") {
  auto m = nr_bb::fapi_from_json_line("{\"type\":\"CONFIG_REQ\",\"sfn\":1,\"slot\":2,\"handle\":9}");
  REQUIRE(m.type == nr_bb::FapiMsgType::CONFIG_REQ);
  REQUIRE(m.sfn == 1);
  REQUIRE(m.slot == 2);
  auto line = nr_bb::fapi_to_json_line(m);
  REQUIRE(line.find("CONFIG_REQ") != std::string::npos);
}
