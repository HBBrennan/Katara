//
//  shared_pointer_impl.cc
//  Katara
//
//  Created by Arne Philipeit on 10/23/22.
//  Copyright © 2022 Arne Philipeit. All rights reserved.
//

#include "shared_pointer_impl.h"

#include <fstream>
#include <sstream>
#include <vector>

#include "src/lang/processors/ir/serialization/parse.h"

namespace lang {
namespace ir_lowerers {

SharedPointerLoweringFuncs AddSharedPointerLoweringFuncsToProgram(ir::Program* program) {
  std::ifstream fstream("src/lang/processors/ir/lowerers/shared_pointer_impl.ir");
  std::stringstream sstream;
  sstream << fstream.rdbuf();
  std::vector<ir::Func*> funcs =
      lang::ir_serialization::ParseAdditionalFuncsForProgramOrDie(program, sstream.str());
  return SharedPointerLoweringFuncs{
      .make_shared_func_num = funcs.at(0)->number(),
      .strong_copy_shared_func_num = funcs.at(1)->number(),
      .weak_copy_shared_func_num = funcs.at(2)->number(),
      .delete_ptr_to_strong_shared_func_num = funcs.at(3)->number(),
      .delete_strong_shared_func_num = funcs.at(4)->number(),
      .delete_ptr_to_weak_shared_func_num = funcs.at(5)->number(),
      .delete_weak_shared_func_num = funcs.at(6)->number(),
      .validate_weak_shared_func_num = funcs.at(7)->number(),
  };
}

}  // namespace ir_lowerers
}  // namespace lang
