#include "nr_bb/harq.hpp"
#include "nr_bb/llr.hpp"

namespace nr_bb {

void HarqBuffer::upsert(uint32_t process_id, int ndi, const SoftVec& llr, int rv) {
  auto it = procs_.find(process_id);
  if (it == procs_.end() || it->second.ndi != ndi) {
    HarqProcess p;
    p.process_id = process_id;
    p.ndi = ndi;
    p.soft_buffer = llr;
    p.rv = rv;
    p.tx_count = 1;
    procs_[process_id] = std::move(p);
    return;
  }
  require(it->second.soft_buffer.size() == llr.size(), "HARQ LLR size mismatch");
  it->second.soft_buffer = llr_combine(it->second.soft_buffer, llr);
  it->second.rv = rv;
  it->second.tx_count += 1;
}

std::optional<SoftVec> HarqBuffer::get(uint32_t process_id) const {
  auto it = procs_.find(process_id);
  if (it == procs_.end()) return std::nullopt;
  return it->second.soft_buffer;
}

void HarqBuffer::clear(uint32_t process_id) { procs_.erase(process_id); }

}  // namespace nr_bb
