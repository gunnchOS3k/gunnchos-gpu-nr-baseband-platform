#include "nr_bb/layer_mapping.hpp"

namespace nr_bb {

std::vector<ComplexVec> layer_map(const ComplexVec& symbols, int n_layers) {
  require(n_layers == 1 || n_layers == 2 || n_layers == 4, "n_layers must be 1,2,4");
  require(symbols.size() % static_cast<size_t>(n_layers) == 0, "symbol count not divisible by layers");
  const std::size_t n_sym = symbols.size() / static_cast<size_t>(n_layers);
  std::vector<ComplexVec> layers(static_cast<size_t>(n_layers), ComplexVec(n_sym));
  for (std::size_t t = 0; t < n_sym; ++t) {
    for (int l = 0; l < n_layers; ++l) {
      layers[static_cast<size_t>(l)][t] = symbols[t * static_cast<size_t>(n_layers) + static_cast<size_t>(l)];
    }
  }
  return layers;
}

ComplexVec layer_demap(const std::vector<ComplexVec>& layers) {
  require(!layers.empty(), "no layers");
  const std::size_t n_sym = layers[0].size();
  for (const auto& L : layers) require(L.size() == n_sym, "layer length mismatch");
  ComplexVec out;
  out.reserve(n_sym * layers.size());
  for (std::size_t t = 0; t < n_sym; ++t)
    for (const auto& L : layers) out.push_back(L[t]);
  return out;
}

}  // namespace nr_bb
