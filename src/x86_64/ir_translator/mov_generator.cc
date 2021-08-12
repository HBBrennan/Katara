//
//  mov_generator.cc
//  Katara
//
//  Created by Arne Philipeit on 8/11/21.
//  Copyright © 2021 Arne Philipeit. All rights reserved.
//

#include "mov_generator.h"

#include "src/x86_64/instrs/data_instrs.h"
#include "src/x86_64/instrs/instr.h"
#include "src/x86_64/ir_translator/size_translator.h"
#include "src/x86_64/ir_translator/temporary_reg.h"
#include "src/x86_64/ir_translator/value_translator.h"
#include "src/x86_64/ops.h"

namespace ir_to_x86_64_translator {

void GenerateMov(x86_64::RM x86_64_result, x86_64::Operand x86_64_origin, ir::Instr* instr,
                 BlockContext& ctx) {
  if (x86_64_result == x86_64_origin) {
    return;
  }

  if ((x86_64_origin.is_imm() && x86_64_origin.size() == x86_64::k64) ||
      (x86_64_result.is_mem() && x86_64_origin.is_mem())) {
    TemporaryReg tmp =
        TemporaryReg::ForOperand(x86_64_origin, /*can_be_result_reg=*/false, instr, ctx);

    ctx.x86_64_block()->AddInstr<x86_64::Mov>(x86_64_result, tmp.reg());

    tmp.Restore(ctx);
  } else {
    ctx.x86_64_block()->AddInstr<x86_64::Mov>(x86_64_result, x86_64_origin);
  }
}

}  // namespace ir_to_x86_64_translator
