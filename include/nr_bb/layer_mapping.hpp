#pragma once
/**
 * @file layer_mapping.hpp
 * @brief Layer mapping / demapping for 1/2/4 layers.
 *
 * TRACEABILITY NOTE: Conceptually TS 38.211 §6.3.1.3 layer mapping.
 * Educational round-robin mapping; NOT full NR codebook/precoding conformance.
 */
#include "nr_bb/types.hpp"
#include <cstddef>
#include <vector>

namespace nr_bb {

/** Map symbols to layers: result[layer][time]. */
std::vector<ComplexVec> layer_map(const ComplexVec& symbols, int n_layers);
ComplexVec layer_demap(const std::vector<ComplexVec>& layers);

}  // namespace nr_bb
