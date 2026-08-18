# Class — PHY / data (current)

Conceptual types in `include/nr_bb/`. Not a UML export of every function.

```mermaid
classDiagram
  class Types {
    Complex
    ComplexVec
    BitVec
    SoftVec
    ModulationOrder
    EqualizerType
  }
  class LdpcParams {
    BaseGraph bg
    int zc
    int max_iter
    int info_bits
  }
  class LdpcGraphInfo {
    int mb nb kb zc
    string provenance
  }
  class FapiMessage {
    uint16 version
    FapiMsgType type
    uint32 sfn slot handle
    BitVec payload
  }
  class FapiSession {
    FapiState state_
    handle(FapiMessage)
  }
  class DeadlineReport {
    int mu
    double slot_duration_us
    double median_us p99_us
    string timing_source
    string gpu_claim
  }
  class NumerologyConfig {
    int mu
    double slot_duration_us
  }
  Types <-- LdpcParams
  LdpcParams --> LdpcGraphInfo
  Types <-- FapiMessage
  FapiSession --> FapiMessage
  NumerologyConfig --> DeadlineReport
```

`DeadlineReport.timing_source` defaults to `cpu_synthetic`. The `gpu_claim` string forbids promoting CPU samples to GPU closure.
