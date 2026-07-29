# Helper macros for optional CUDA / sanitizer wiring.
function(nr_bb_report_cuda)
  if(NR_BB_HAS_CUDA)
    message(STATUS "CUDA targets enabled")
  else()
    message(STATUS "CUDA targets disabled (BLOCKED_HARDWARE on GPU gates)")
  endif()
endfunction()
