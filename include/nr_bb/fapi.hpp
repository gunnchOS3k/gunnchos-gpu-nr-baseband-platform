#pragma once
/**
 * @file fapi.hpp
 * @brief Bounded FAPI-like stateful control/data interface.
 *
 * TRACEABILITY NOTE: Inspired by Small Cell Forum FAPI / 5G FAPI message
 * families (CONFIG, SLOT, UL/DL_TTI.req, TX/RX_DATA, CRC.ind, ERROR.ind).
 * This is a research-bounded state machine — NOT SCF FAPI conformance.
 */
#include "nr_bb/types.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace nr_bb {

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

 private:
  FapiState state_ = FapiState::Idle;
  uint32_t slot_count_ = 0;
  uint32_t sfn_ = 0;
  uint32_t slot_ = 0;
};

/** Parse minimal JSON fixture fields (key:value pairs, educational). */
FapiMessage fapi_from_json_line(const std::string& line);
std::string fapi_to_json_line(const FapiMessage& m);

}  // namespace nr_bb
