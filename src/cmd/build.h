//
//  build.h
//  Katara
//
//  Created by Arne Philipeit on 12/19/21.
//  Copyright © 2021 Arne Philipeit. All rights reserved.
//

#ifndef cmd_build_h
#define cmd_build_h

#include <memory>
#include <variant>

#include "src/cmd/context.h"
#include "src/cmd/error_codes.h"
#include "src/ir/representation/program.h"
#include "src/x86_64/program.h"

namespace cmd {

struct BuildResult {
  std::unique_ptr<ir::Program> ir_program;
  std::unique_ptr<x86_64::Program> x86_64_program;
};

std::variant<BuildResult, ErrorCode> Build(Context* ctx);

}  // namespace cmd

#endif /* cmd_build_h */