//
//  parser_fuzz_test.cc
//  Katara
//
//  Created by Arne Philipeit on 12/19/20.
//  Copyright © 2020 Arne Philipeit. All rights reserved.
//

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "src/common/positions/positions.h"
#include "src/lang/processors/scanner/scanner.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  using namespace lang;

  std::string contents(reinterpret_cast<const char*>(data), size);

  common::positions::FileSet pos_file_set;
  common::positions::File* pos_file = pos_file_set.AddFile("test_file.kat", contents);

  scanner::Scanner scanner(pos_file);

  while (scanner.token() != tokens::kEOF) {
    scanner.Next();
  }

  return 0;
}
