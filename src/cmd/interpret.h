//
//  interpret.h
//  Katara
//
//  Created by Arne Philipeit on 1/30/22.
//  Copyright © 2022 Arne Philipeit. All rights reserved.
//

#ifndef cmd_interpret_h
#define cmd_interpret_h

#include <filesystem>
#include <vector>

#include "src/cmd/build.h"
#include "src/cmd/context/context.h"
#include "src/cmd/debug.h"
#include "src/cmd/error_codes.h"

namespace cmd {

ErrorCode Interpret(std::vector<std::filesystem::path>& paths, BuildOptions& options,
                    DebugHandler& debug_handler, Context* ctx);

}

#endif /* cmd_interpret_h */