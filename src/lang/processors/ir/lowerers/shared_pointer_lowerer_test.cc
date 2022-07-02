//
//  shared_pointer_lowerer_test.cc
//  Katara
//
//  Created by Arne Philipeit on 6/26/22.
//  Copyright © 2022 Arne Philipeit. All rights reserved.
//

#include "src/lang/processors/ir/lowerers/shared_pointer_lowerer.h"

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "src/ir/representation/num_types.h"
#include "src/ir/representation/program.h"
#include "src/ir/serialization/print.h"
#include "src/lang/processors/ir/checker/checker.h"
#include "src/lang/processors/ir/serialization/parse.h"

using ::testing::Each;
using ::testing::Property;

struct LowererTestParams {
  std::string input_program;
  std::string expected_program;
};

class SharedPointerLowererTest : public testing::TestWithParam<LowererTestParams> {};

INSTANTIATE_TEST_SUITE_P(SharedPointerLowererTestInstance, SharedPointerLowererTest,
                         testing::Values(
                             LowererTestParams{
                                 .input_program = R"ir(
@0 f(%0:lshared_ptr<u16, s>) => (u16) {
  {0}
    %1:u16 = load %0
    ret %1
}
)ir",
                                 .expected_program = R"ir(
@0 f(%2:ptr, %3:ptr) => (u16) {
  {0}
    %1:u16 = load %3
    ret %1
}
)ir",
                             },
                             LowererTestParams{
                                 .input_program = R"ir(
@0 f() => (lshared_ptr<func, s>) {
  {0}
    %0:lshared_ptr<func, s> = make_shared #1:i64
    store %0, @0
    ret %0
}
)ir",
                                 .expected_program = R"ir(
@0 f() => (ptr, ptr) {
  {0}
    %1:ptr, %2:ptr = call @1, #8:i64, @-1
    store %2, @0
    ret %1, %2
}
)ir",
                             },
                             LowererTestParams{
                                 .input_program = R"ir(
@0 f(%0:i16, %1:lshared_ptr<u32, w>, %2:b) => (lshared_ptr<i16, s>, ptr) {
  {0}
    %3:lshared_ptr<i16, s> = make_shared #1:i64
    %4:lshared_ptr<u32, w> = copy_shared %1, #0:i64
    store %3, %0
    delete_shared %1
    delete_shared %4
    ret %3, 0x0
}

@1 g() => () {
  {42}
    %0:lshared_ptr<u32, s> = make_shared #1:i64
    %1:lshared_ptr<u32, w> = copy_shared %0, #0:i64
    %2:lshared_ptr<i16, s>, %3:ptr = call @0, #1234:i16, %1, #t
    delete_shared %0
    delete_shared %2
    ret
}

)ir",
                                 .expected_program = R"ir(
@0 f (%0:i16, %5:ptr, %6:ptr, %2:b) => (ptr, ptr, ptr) {
  {0}
    %7:ptr, %8:ptr = call @2, #8:i64, @-1
    %9:ptr = call @4, %5, %6, #0:i64
    store %8, %0
    call @6, %5
    call @6, %5
    ret %7, %8, 0x0
}

@1 g () => () {
  {42}
    %4:ptr, %5:ptr = call @2, #8:i64, @-1
    %6:ptr = call @4, %4, %5, #0:i64
    %7:ptr, %8:ptr, %3:ptr = call @0, #1234:i16, %4, %6, #t
    call @5, %4
    call @5, %7
    ret
}
)ir",
                             },
                             LowererTestParams{
                                 .input_program = R"ir(
@0 f() => (lshared_ptr<u16, s>) {
  {0}
    %0:lshared_ptr<u16, s> = make_shared #1:i64
    %1:lshared_ptr<u16, s> = mov %0
    ret %1
}
)ir",
                                 .expected_program = R"ir(
@0 f() => (ptr, ptr) {
  {0}
    %2:ptr, %3:ptr = call @1, #8:i64, @-1
    ret %2, %3
}
)ir",
                             },
                             LowererTestParams{
                                 .input_program = R"ir(
@0 f(%0:b) => (lshared_ptr<u64, s>) {
  {0}
    jcc %0, {1}, {2}
  {1}
    %1:lshared_ptr<u64, s> = make_shared #1:i64
    jmp {3}
  {2}
    %2:lshared_ptr<u64, s> = make_shared #1:i64
    jmp {3}
  {3}
    %3:lshared_ptr<u64, s> = phi %1{1}, %2{2}
    ret %3
}
)ir",
                                 .expected_program = R"ir(
@0 f(%0:b) => (ptr, ptr) {
  {0}
    jcc %0, {1}, {2}
  {1}
    %8:ptr, %9:ptr = call @1, #8:i64, @-1
    jmp {3}
  {2}
    %4:ptr, %5:ptr = call @1, #8:i64, @-1
    jmp {3}
  {3}
    %6:ptr = phi %8{1}, %4{2}
    %7:ptr = phi %9{1}, %5{2}
    ret %6, %7
}
)ir",
                             },
                             LowererTestParams{
                                 .input_program = R"ir(
@0 f(%0:b) => (lshared_ptr<u64, s>) {
  {0}
    %1:lshared_ptr<u64, s> = make_shared #1:i64
    jmp{1}
  {1}
    %2:lshared_ptr<u64, s> = phi %1{0}, %3{2}
    jcc %0, {2}, {3}
  {2}
    %3:lshared_ptr<u64, s> = copy_shared %2, #0:i64
    delete_shared %2
    jmp {1}
  {3}
    ret %2
}
)ir",
                                 .expected_program = R"ir(
@0 f(%0:b) => (ptr, ptr) {
  {0}
    %4:ptr, %5:ptr = call @1, #8:i64, @-1
    jmp{1}
  {1}
    %6:ptr = phi %4{0}, %6{2}
    %7:ptr = phi %5{0}, %8{2}
    jcc %0, {2}, {3}
  {2}
    %8:ptr = call @2, %6, %7, #0:i64
    call @4, %6
    jmp {1}
  {3}
    ret %6, %7
}
)ir",
                             },
                             LowererTestParams{
                                 .input_program = R"ir(
@0 main() => (i64) {
  {0}
    %0:lshared_ptr<i64, s> = make_shared #1:i64
    store %0, #0:i64
    %1:lshared_ptr<i64, s> = make_shared #1:i64
    store %1, #0:i64
    jmp {1}
  {1}
    %2:i64 = load %1
    %3:b = ilss %2, #10:i64
    jcc %3, {4}, {3}
  {2}
    %4:i64 = load %1
    %5:i64 = iadd %4, #1:i64
    store %1, %5
    jmp {1}
  {3}
    delete_shared %1
    %9:i64 = load %0
    delete_shared %0
    ret %9
  {4}
    %6:i64 = load %1
    %7:i64 = load %0
    %8:i64 = iadd %7, %6
    store %0, %8
    jmp {2}
}
)ir",
                                 .expected_program = R"ir(
@0 main () => (i64) {
{0}
  %10:ptr, %11:ptr = call @1, #8:i64, @-1
  store %11, #0:i64
  %12:ptr, %13:ptr = call @1, #8:i64, @-1
  store %13, #0:i64
  jmp {1}
{1}
  %2:i64 = load %13
  %3:b = ilss %2, #10:i64
  jcc %3, {4}, {3}
{2}
  %4:i64 = load %13
  %5:i64 = iadd %4, #1:i64
  store %13, %5
  jmp {1}
{3}
  call @4, %12
  %9:i64 = load %11
  call @4, %10
  ret %9
{4}
  %6:i64 = load %13
  %7:i64 = load %11
  %8:i64 = iadd %7, %6
  store %11, %8
  jmp {2}
}
)ir",
                             }));

TEST_P(SharedPointerLowererTest, LowersProgram) {
  std::unique_ptr<ir::Program> lowered_program =
      lang::ir_serialization::ParseProgram(GetParam().input_program);
  std::unique_ptr<ir::Program> expected_program =
      lang::ir_serialization::ParseProgram(GetParam().expected_program);
  lang::ir_checker::AssertProgramIsOkay(lowered_program.get());
  ASSERT_THAT(lang::ir_checker::CheckProgram(expected_program.get()),
              Each(Property(&ir_checker::Issue::kind,
                            ir_checker::Issue::Kind::kCallInstrStaticCalleeDoesNotExist)));

  lang::ir_lowerers::LowerSharedPointersInProgram(lowered_program.get());
  lang::ir_checker::AssertProgramIsOkay(lowered_program.get());
  for (ir::func_num_t func_num = 0; func_num < ir::func_num_t(expected_program->funcs().size());
       func_num++) {
    EXPECT_TRUE(
        ir::IsEqual(lowered_program->GetFunc(func_num), expected_program->GetFunc(func_num)))
        << "Expected different lowered function, got:\n"
        << ir_serialization::Print(lowered_program->GetFunc(func_num)) << "\nexpected:\n"
        << ir_serialization::Print(expected_program->GetFunc(func_num));
  }
}
