#include "nr_bb/fapi.hpp"
#include <cctype>
#include <sstream>

namespace nr_bb {

namespace {
std::string type_name(FapiMsgType t) {
  switch (t) {
    case FapiMsgType::CONFIG_REQ: return "CONFIG_REQ";
    case FapiMsgType::CONFIG_RESP: return "CONFIG_RESP";
    case FapiMsgType::START_REQ: return "START_REQ";
    case FapiMsgType::STOP_REQ: return "STOP_REQ";
    case FapiMsgType::SLOT_IND: return "SLOT_IND";
    case FapiMsgType::UL_TTI_REQ: return "UL_TTI_REQ";
    case FapiMsgType::DL_TTI_REQ: return "DL_TTI_REQ";
    case FapiMsgType::TX_DATA_REQ: return "TX_DATA_REQ";
    case FapiMsgType::RX_DATA_IND: return "RX_DATA_IND";
    case FapiMsgType::CRC_IND: return "CRC_IND";
    case FapiMsgType::ERROR_IND: return "ERROR_IND";
  }
  return "UNKNOWN";
}

std::optional<FapiMsgType> parse_type(const std::string& s) {
  if (s == "CONFIG_REQ") return FapiMsgType::CONFIG_REQ;
  if (s == "CONFIG_RESP") return FapiMsgType::CONFIG_RESP;
  if (s == "START_REQ") return FapiMsgType::START_REQ;
  if (s == "STOP_REQ") return FapiMsgType::STOP_REQ;
  if (s == "SLOT_IND") return FapiMsgType::SLOT_IND;
  if (s == "UL_TTI_REQ") return FapiMsgType::UL_TTI_REQ;
  if (s == "DL_TTI_REQ") return FapiMsgType::DL_TTI_REQ;
  if (s == "TX_DATA_REQ") return FapiMsgType::TX_DATA_REQ;
  if (s == "RX_DATA_IND") return FapiMsgType::RX_DATA_IND;
  if (s == "CRC_IND") return FapiMsgType::CRC_IND;
  if (s == "ERROR_IND") return FapiMsgType::ERROR_IND;
  return std::nullopt;
}
}  // namespace

void FapiSession::reset() {
  state_ = FapiState::Idle;
  slot_count_ = 0;
  sfn_ = 0;
  slot_ = 0;
}

FapiMessage FapiSession::handle(const FapiMessage& in) {
  FapiMessage out = in;
  auto err = [&](const std::string& e) {
    out.type = FapiMsgType::ERROR_IND;
    out.error = e;
    out.version = FAPI_MSG_VERSION;
    state_ = FapiState::Error;
    return out;
  };
  if (in.version != 0 && in.version != FAPI_MSG_VERSION) {
    return err("unsupported FAPI message version");
  }

  switch (in.type) {
    case FapiMsgType::CONFIG_REQ:
      if (state_ != FapiState::Idle && state_ != FapiState::Configured && state_ != FapiState::Error)
        return err("CONFIG_REQ invalid in current state");
      state_ = FapiState::Configured;
      out.type = FapiMsgType::CONFIG_RESP;
      out.version = FAPI_MSG_VERSION;
      out.json_meta = "{\"status\":\"ok\",\"version\":1}";
      return out;
    case FapiMsgType::START_REQ:
      if (state_ != FapiState::Configured) return err("START_REQ requires Configured");
      state_ = FapiState::Running;
      out.type = FapiMsgType::SLOT_IND;
      out.version = FAPI_MSG_VERSION;
      out.sfn = sfn_;
      out.slot = slot_;
      return out;
    case FapiMsgType::STOP_REQ:
      if (state_ != FapiState::Running && state_ != FapiState::Error)
        return err("STOP_REQ invalid state");
      state_ = FapiState::Configured;
      out.type = FapiMsgType::CONFIG_RESP;
      out.version = FAPI_MSG_VERSION;
      return out;
    case FapiMsgType::UL_TTI_REQ:
    case FapiMsgType::DL_TTI_REQ:
    case FapiMsgType::TX_DATA_REQ:
      if (state_ != FapiState::Running) return err("data/tti requires Running");
      ++slot_count_;
      slot_ = (slot_ + 1) % 20;
      if (slot_ == 0) sfn_ = (sfn_ + 1) % 1024;
      out.type = (in.type == FapiMsgType::TX_DATA_REQ) ? FapiMsgType::CRC_IND : FapiMsgType::SLOT_IND;
      out.version = FAPI_MSG_VERSION;
      out.sfn = sfn_;
      out.slot = slot_;
      out.crc_ok = true;
      if (out.type == FapiMsgType::CRC_IND) out.payload = in.payload;
      return out;
    case FapiMsgType::SLOT_IND:
    case FapiMsgType::RX_DATA_IND:
    case FapiMsgType::CRC_IND:
    case FapiMsgType::CONFIG_RESP:
    case FapiMsgType::ERROR_IND:
      return err("unexpected message direction for session.handle");
  }
  return err("unknown message");
}

FapiMessage fapi_from_json_line(const std::string& line) {
  FapiMessage m;
  auto get = [&](const std::string& key) -> std::string {
    const auto k = "\"" + key + "\"";
    auto p = line.find(k);
    if (p == std::string::npos) return {};
    p = line.find(':', p);
    if (p == std::string::npos) return {};
    ++p;
    while (p < line.size() && (line[p] == ' ' || line[p] == '\"')) {
      if (line[p] == '\"') {
        const auto end = line.find('\"', p + 1);
        return line.substr(p + 1, end - p - 1);
      }
      ++p;
    }
    std::size_t end = p;
    while (end < line.size() && (isdigit(static_cast<unsigned char>(line[end])) || line[end] == '-')) ++end;
    return line.substr(p, end - p);
  };
  const auto t = parse_type(get("type"));
  require(t.has_value(), "invalid FAPI type in JSON");
  m.type = *t;
  if (auto s = get("version"); !s.empty()) m.version = static_cast<uint16_t>(std::stoul(s));
  if (auto s = get("sfn"); !s.empty()) m.sfn = static_cast<uint32_t>(std::stoul(s));
  if (auto s = get("slot"); !s.empty()) m.slot = static_cast<uint32_t>(std::stoul(s));
  if (auto s = get("handle"); !s.empty()) m.handle = static_cast<uint32_t>(std::stoul(s));
  m.json_meta = line;
  return m;
}

std::string fapi_to_json_line(const FapiMessage& m) {
  std::ostringstream oss;
  oss << "{\"version\":" << m.version << ",\"type\":\"" << type_name(m.type) << "\",\"sfn\":" << m.sfn
      << ",\"slot\":" << m.slot << ",\"handle\":" << m.handle << ",\"crc_ok\":" << (m.crc_ok ? "true" : "false");
  if (!m.error.empty()) oss << ",\"error\":\"" << m.error << "\"";
  oss << "}";
  return oss.str();
}

int fapi_run_fixture_lines(const std::vector<std::string>& lines, FapiSession& session) {
  int errors = 0;
  for (const auto& line : lines) {
    try {
      auto m = fapi_from_json_line(line);
      auto out = session.handle(m);
      if (out.type == FapiMsgType::ERROR_IND) ++errors;
    } catch (...) {
      ++errors;
    }
  }
  return errors;
}

}  // namespace nr_bb
