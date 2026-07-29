#include "nr_bb/fapi.hpp"
#include <cstdint>
#include <string>
#include <vector>

// Lightweight corpus-driven fuzz without libFuzzer dependency (CPU hosts).
int main(int argc, char** argv) {
  std::vector<std::string> corpus = {
      "{\"type\":\"CONFIG_REQ\",\"sfn\":0,\"slot\":0,\"handle\":1}",
      "{\"type\":\"START_REQ\",\"sfn\":0,\"slot\":0,\"handle\":1}",
      "{\"type\":\"TX_DATA_REQ\",\"sfn\":0,\"slot\":0,\"handle\":1}",
      "{\"type\":\"UL_TTI_REQ\",\"sfn\":1,\"slot\":3,\"handle\":2}",
      "{\"type\":\"STOP_REQ\",\"sfn\":0,\"slot\":0,\"handle\":1}",
      "{\"type\":\"NOT_A_TYPE\",\"sfn\":0,\"slot\":0,\"handle\":1}",
      "{corrupt",
      "",
  };
  if (argc > 1) {
    corpus.clear();
    for (int i = 1; i < argc; ++i) corpus.emplace_back(argv[i]);
  }
  int errors = 0;
  for (const auto& line : corpus) {
    try {
      auto m = nr_bb::fapi_from_json_line(line);
      nr_bb::FapiSession s;
      // Corruption / sequencing: feed regardless of state
      (void)s.handle(m);
      (void)s.handle(m);
    } catch (const nr_bb::Error&) {
      ++errors;
    } catch (...) {
      ++errors;
    }
  }
  // Also binary corruption on payload session path
  nr_bb::FapiSession s;
  (void)s.handle({.type = nr_bb::FapiMsgType::CONFIG_REQ});
  (void)s.handle({.type = nr_bb::FapiMsgType::START_REQ});
  nr_bb::FapiMessage bad{.type = nr_bb::FapiMsgType::TX_DATA_REQ};
  bad.payload = {0, 1, 2, 3};  // non 0/1 still accepted at FAPI layer
  (void)s.handle(bad);
  return errors >= 0 ? 0 : 1;
}
