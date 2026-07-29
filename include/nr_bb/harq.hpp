#pragma once
/**
 * @file harq.hpp
 * @brief HARQ soft-buffer interface (Chase combining).
 *
 * TRACEABILITY NOTE: Conceptually related to TS 38.321 HARQ and soft combining.
 * Educational buffer API only — NOT MAC/PHY HARQ process conformance.
 */
#include "nr_bb/types.hpp"
#include <cstdint>
#include <optional>
#include <unordered_map>

namespace nr_bb {

struct HarqProcess {
  uint32_t process_id = 0;
  int ndi = 0;
  SoftVec soft_buffer;
  int rv = 0;
  int tx_count = 0;
};

class HarqBuffer {
 public:
  void upsert(uint32_t process_id, int ndi, const SoftVec& llr, int rv);
  std::optional<SoftVec> get(uint32_t process_id) const;
  void clear(uint32_t process_id);
  std::size_t size() const { return procs_.size(); }

 private:
  std::unordered_map<uint32_t, HarqProcess> procs_;
};

}  // namespace nr_bb
