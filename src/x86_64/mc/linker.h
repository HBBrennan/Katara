//
//  linker.h
//  Katara
//
//  Created by Arne Philipeit on 11/27/19.
//  Copyright © 2019 Arne Philipeit. All rights reserved.
//

#ifndef x86_64_linker_h
#define x86_64_linker_h

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/data.h"
#include "x86_64/ops.h"

namespace x86_64 {

class Linker {
 public:
  Linker();
  ~Linker();

  void AddFuncAddr(int64_t func_id, uint8_t* func_addr);
  void AddBlockAddr(int64_t block_id, uint8_t* block_addr);

  void AddFuncRef(const FuncRef& func_ref, common::data patch_data);
  void AddBlockRef(const BlockRef& block_ref, common::data patch_data);

  void ApplyPatches() const;

 private:
  std::unordered_map<int64_t, uint8_t*> func_addrs_;
  std::unordered_map<int64_t, uint8_t*> block_addrs_;

  struct FuncPatch {
    FuncRef func_ref;
    common::data patch_data;
  };
  struct BlockPatch {
    BlockRef block_ref;
    common::data patch_data;
  };

  std::vector<FuncPatch> func_patches_;
  std::vector<BlockPatch> block_patches_;
};

}  // namespace x86_64

#endif /* x86_64_linker_h */
