#include <catch2/catch_test_macros.hpp>
#include "nr_bb/fapi.hpp"

TEST_CASE("FAPI state machine happy path", "[fapi]") {
  nr_bb::FapiSession s;
  REQUIRE(s.state() == nr_bb::FapiState::Idle);
  auto r1 = s.handle({.type = nr_bb::FapiMsgType::CONFIG_REQ});
  REQUIRE(r1.type == nr_bb::FapiMsgType::CONFIG_RESP);
  REQUIRE(r1.version == nr_bb::FAPI_MSG_VERSION);
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

TEST_CASE("FAPI versioned JSON fixtures", "[fapi]") {
  auto m = nr_bb::fapi_from_json_line(
      "{\"version\":1,\"type\":\"CONFIG_REQ\",\"sfn\":1,\"slot\":2,\"handle\":9}");
  REQUIRE(m.type == nr_bb::FapiMsgType::CONFIG_REQ);
  REQUIRE(m.version == 1);
  REQUIRE(m.sfn == 1);
  REQUIRE(m.slot == 2);
  auto line = nr_bb::fapi_to_json_line(m);
  REQUIRE(line.find("CONFIG_REQ") != std::string::npos);
  REQUIRE(line.find("\"version\":") != std::string::npos);
}

TEST_CASE("FAPI fixture runner", "[fapi]") {
  nr_bb::FapiSession s;
  std::vector<std::string> lines = {
      "{\"version\":1,\"type\":\"CONFIG_REQ\",\"sfn\":0,\"slot\":0,\"handle\":1}",
      "{\"version\":1,\"type\":\"START_REQ\",\"sfn\":0,\"slot\":0,\"handle\":1}",
      "{\"version\":1,\"type\":\"TX_DATA_REQ\",\"sfn\":0,\"slot\":0,\"handle\":1}",
  };
  REQUIRE(nr_bb::fapi_run_fixture_lines(lines, s) == 0);
}

TEST_CASE("FAPI rejects wrong version", "[fapi]") {
  nr_bb::FapiSession s;
  nr_bb::FapiMessage m{.version = 99, .type = nr_bb::FapiMsgType::CONFIG_REQ};
  auto r = s.handle(m);
  REQUIRE(r.type == nr_bb::FapiMsgType::ERROR_IND);
}
