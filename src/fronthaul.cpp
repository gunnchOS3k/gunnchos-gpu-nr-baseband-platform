#include "nr_bb/fronthaul.hpp"
#include <algorithm>
#include <fstream>
#include <random>

namespace nr_bb {

FhPacket FronthaulEmulator::make_u_plane(const ComplexVec& iq, uint16_t pc_id, uint16_t seq,
                                         uint64_t t_ns) {
  FhPacket p;
  p.meta.pc_id = pc_id;
  p.meta.seq_id = seq;
  p.meta.msg_type = 0;  // IQ
  p.meta.timestamp_ns = t_ns;
  p.meta.plane = Plane::U;
  p.iq = iq;
  p.raw.resize(16 + iq.size() * 16);
  // header
  p.raw[0] = static_cast<uint8_t>(p.meta.msg_type);
  p.raw[1] = static_cast<uint8_t>(p.meta.plane);
  p.raw[2] = static_cast<uint8_t>(pc_id & 0xff);
  p.raw[3] = static_cast<uint8_t>((pc_id >> 8) & 0xff);
  p.raw[4] = static_cast<uint8_t>(seq & 0xff);
  p.raw[5] = static_cast<uint8_t>((seq >> 8) & 0xff);
  return p;
}

std::vector<FhPacket> FronthaulEmulator::apply_impairments(const std::vector<FhPacket>& in) {
  std::mt19937 rng(imp_.seed);
  std::uniform_real_distribution<double> U(0.0, 1.0);
  std::vector<FhPacket> out;
  out.reserve(in.size());
  for (auto p : in) {
    if (U(rng) < imp_.loss_prob) continue;
    if (U(rng) < imp_.corrupt_prob && !p.iq.empty()) {
      p.iq[0] += Complex{10.0, -10.0};
      if (!p.raw.empty()) p.raw[0] ^= 0xFF;
    }
    out.push_back(std::move(p));
  }
  if (imp_.reorder_prob > 0 && out.size() >= 2 && U(rng) < imp_.reorder_prob) {
    std::swap(out[0], out[out.size() - 1]);
  }
  if (imp_.delay_packets > 0 && !out.empty()) {
    // Educational: rotate left by delay to emulate reordering/delay queue
    const int d = imp_.delay_packets % static_cast<int>(out.size());
    std::rotate(out.begin(), out.begin() + d, out.end());
  }
  return out;
}

std::vector<FhPacket> FronthaulEmulator::loopback(const std::vector<FhPacket>& in) {
  return apply_impairments(in);
}

void FronthaulEmulator::write_pcap_dump(const std::string& path, const std::vector<FhPacket>& pkts) {
  std::ofstream f(path, std::ios::binary);
  require(f.good(), "cannot open pcap dump path");
  // Minimal pcap global header + packets (educational)
  const uint8_t gh[24] = {0xd4, 0xc3, 0xb2, 0xa1, 0x02, 0x00, 0x04, 0x00, 0, 0, 0, 0,
                          0,    0,    0,    0,    0xff, 0xff, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
  f.write(reinterpret_cast<const char*>(gh), 24);
  for (const auto& p : pkts) {
    const uint32_t len = static_cast<uint32_t>(p.raw.size());
    uint8_t ph[16] = {};
    // incl_len / orig_len
    ph[8] = len & 0xff;
    ph[9] = (len >> 8) & 0xff;
    ph[10] = (len >> 16) & 0xff;
    ph[11] = (len >> 24) & 0xff;
    ph[12] = ph[8];
    ph[13] = ph[9];
    ph[14] = ph[10];
    ph[15] = ph[11];
    f.write(reinterpret_cast<const char*>(ph), 16);
    if (!p.raw.empty()) f.write(reinterpret_cast<const char*>(p.raw.data()), static_cast<std::streamsize>(p.raw.size()));
  }
}

}  // namespace nr_bb
