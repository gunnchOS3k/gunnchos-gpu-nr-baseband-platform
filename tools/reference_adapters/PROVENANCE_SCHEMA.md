"""
Provenance schema for tools/reference_adapters/*

Required keys:
  adapter: sionna | srsran | oai | matlab
  status: REFERENCE_ADAPTER_IMPLEMENTED | REFERENCE_DEPENDENCY_UNAVAILABLE
          | COMPARISON_PASS | COMPARISON_FAIL
  dependency: human-readable dependency name
  vectors: list of vector / campaign ids
  metrics: object (may be empty)
  notes: string
"""
