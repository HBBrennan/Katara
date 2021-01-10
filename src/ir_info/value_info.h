//
//  value_info.h
//  Katara
//
//  Created by Arne Philipeit on 1/25/20.
//  Copyright © 2020 Arne Philipeit. All rights reserved.
//

#ifndef ir_info_value_info_h
#define ir_info_value_info_h

#include <unordered_map>
#include <unordered_set>

#include "ir/block.h"
#include "ir/func.h"
#include "ir/instr.h"
#include "ir/value.h"

namespace ir_info {

class ValueInfo {
 public:
  ValueInfo();
  ~ValueInfo();

  ir::Instr* GetDefiningInstr(ir::Computed value) const;
  void SetDefiningInstr(ir::Computed value, ir::Instr* instr);

  const std::unordered_set<ir::Instr*>& GetUsingInstrs(ir::Computed value) const;
  void AddUsingInstr(ir::Computed value, ir::Instr* instr);

 private:
  std::unordered_map<ir::Computed, ir::Instr*> defining_instrs_;
  std::unordered_map<ir::Computed, std::unordered_set<ir::Instr*>> using_instrs_;
};

}  // namespace ir_info

#endif /* ir_info_value_info_h */
