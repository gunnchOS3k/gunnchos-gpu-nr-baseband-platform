#pragma once
/**
 * @file fapi.hpp
 * @brief Versioned FAPI-like messages + state machine + fixtures.
 */
#include "nr_bb/types.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace nr_bb {

inline constexpr uint16_t FAPI_MSG_VERSION = 1;

enum class FapiState { Idle, Configured, Running, Error };

enum class FapiMsgType {
  CONFIG_REQ,
  CONFIG_RESP,
  START_REQ,
  STOP_REQ,
  SLOT_IND,
  UL_TTI_REQ,
  DL_TTI_REQ,
  TX_DATA_REQ,
  RX_DATA_IND,
  CRC_IND,
  ERROR_IND
};

struct FapiMessage {
  uint16_t version = FAPI_MSG_VERSION;
  FapiMsgType type{};
  uint32_t sfn = 0;
  uint32_t slot = 0;
  uint32_t handle = 0;
  BitVec payload;
  std::string json_meta;
  bool crc_ok = false;
  std::string error;
};

class FapiSession {
 public:
  FapiState state() const { return state_; }
  FapiMessage handle(const FapiMessage& in);
  uint32_t slot_count() const { return slot_count_; }
  void reset();

 private:
  FapiState state_ = FapiState::Idle;
  uint32_t slot_count_ = 0;
  uint32_t sfn_ = 0;
  uint32_t slot_ = 0;
};

FapiMessage fapi_from_json_line(const std::string& line);
std::string fapi_to_json_line(const FapiMessage& m);

/** Run a JSONL fixture sequence; returns ERROR_IND count. */
int fapi_run_fixture_lines(const std::vector<std::string>& lines, FapiSession& session);

}  // namespace nr_bb
