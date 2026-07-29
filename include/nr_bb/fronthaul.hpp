#pragma once
/**
 * @file fronthaul.hpp
 * @brief Fronthaul / RU emulator with C/U/S-plane conceptual separation.
 *
 * TRACEABILITY NOTE: Metadata fields inspired by eCPRI / O-RAN FH concepts.
 * IQ payload replay and impairment injection only — NO RF transmission.
 * See docs/lab/AUTHORIZED_RF_WARNING.md.
 */
#include "nr_bb/types.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace nr_bb {

enum class Plane { C, U, S };

struct EcpriMeta {
  uint16_t pc_id = 0;
  uint16_t seq_id = 0;
  uint8_t msg_type = 0;  // educational
  uint64_t timestamp_ns = 0;
  Plane plane = Plane::U;
};

struct FhPacket {
  EcpriMeta meta;
  ComplexVec iq;
  std::vector<uint8_t> raw;  // serialized educational blob
};

struct FhImpairments {
  double loss_prob = 0.0;
  double reorder_prob = 0.0;
  int delay_packets = 0;
  double corrupt_prob = 0.0;
  uint32_t seed = 1;
};

class FronthaulEmulator {
 public:
  explicit FronthaulEmulator(FhImpairments imp = {}) : imp_(imp) {}
  FhPacket make_u_plane(const ComplexVec& iq, uint16_t pc_id, uint16_t seq, uint64_t t_ns);
  std::vector<FhPacket> apply_impairments(const std::vector<FhPacket>& in);
  std::vector<FhPacket> loopback(const std::vector<FhPacket>& in);
  /** Write minimal PCAP-like hex dump (educational, not full PCAP if empty path skip). */
  void write_pcap_dump(const std::string& path, const std::vector<FhPacket>& pkts);

 private:
  FhImpairments imp_;
};

}  // namespace nr_bb
