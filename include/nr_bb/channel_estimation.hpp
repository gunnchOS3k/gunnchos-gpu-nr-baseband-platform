#pragma once
/**
 * @file channel_estimation.hpp
 * @brief LS pilots + ZF/MMSE scalar channel helpers for the PHY reference path.
 */
#include "nr_bb/types.hpp"
#include <vector>

namespace nr_bb {

ComplexVec ls_channel_estimate(const ComplexVec& rx_dmrs, const ComplexVec& tx_dmrs);
std::vector<ComplexVec> hold_interpolate(const ComplexVec& h_sc, int n_symb);

/** Linear interpolate H across subcarriers given pilot indices. */
ComplexVec interpolate_frequency(const ComplexVec& h_pilots, const std::vector<int>& pilot_idx,
                                 int n_sc);

/** Per-tone ZF equalizer: y / H. */
ComplexVec equalize_zf_tones(const ComplexVec& y, const ComplexVec& H);

/** Per-tone MMSE: y * conj(H) / (|H|^2 + σ²). */
ComplexVec equalize_mmse_tones(const ComplexVec& y, const ComplexVec& H, double noise_var);

}  // namespace nr_bb
