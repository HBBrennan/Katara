//
//  interpreter_integration_test.cc
//  Katara-tests
//
//  Created by Arne Philipeit on 6/17/21.
//  Copyright © 2021 Arne Philipeit. All rights reserved.
//

#include <memory>
#include <string>

#include "gtest/gtest.h"
#include "src/ir/interpreter/interpreter.h"
#include "src/lang/processors/ir_builder/ir_builder.h"
#include "src/lang/processors/packages/mock_loader.h"
#include "src/lang/processors/packages/package.h"
#include "src/lang/processors/packages/package_manager.h"

namespace {

struct ConstantIntExprTestCase {
  const std::string expr;
  const int expected_value;
};

std::ostream& operator<<(std::ostream& os, const ConstantIntExprTestCase& test_case) {
  return os << "TestCase{\"" << test_case.expr << "\", " << test_case.expected_value << "}";
}

class ConstantIntExprTest : public testing::TestWithParam<ConstantIntExprTestCase> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.
};

INSTANTIATE_TEST_SUITE_P(
    ConstantIntExprTestInstance, ConstantIntExprTest,
    testing::Values(ConstantIntExprTestCase{"42 + 24", 66}, ConstantIntExprTestCase{"42 - 24", 18},
                    ConstantIntExprTestCase{"42 * 24", 1008}, ConstantIntExprTestCase{"42 / 24", 1},
                    ConstantIntExprTestCase{"42 % 24", 18}, ConstantIntExprTestCase{"42 & 24", 8},
                    ConstantIntExprTestCase{"42 | 24", 58}, ConstantIntExprTestCase{"42 ^ 24", 50},
                    ConstantIntExprTestCase{"42 &^ 24", 34},
                    ConstantIntExprTestCase{"42 << 2", 168},
                    ConstantIntExprTestCase{"42 >> 2", 10}));

TEST_P(ConstantIntExprTest, ConstantAddition) {
  std::string source = R"kat(
package main

func main() int {
  return )kat" + GetParam().expr +
                       R"kat(
}
  )kat";

  auto pkg_manager = lang::packages::PackageManager(
      /*stdlib_loader=*/nullptr, /*src_loader=*/lang::packages::MockLoaderBuilder()
                                     .SetCurrentDir("/")
                                     .AddSourceFile("/", "add.kat", source)
                                     .Build());

  // Load main package:
  lang::packages::Package* pkg = pkg_manager.LoadMainPackage("/");
  EXPECT_TRUE(pkg_manager.issue_tracker()->issues().empty());
  EXPECT_TRUE(pkg != nullptr);
  EXPECT_TRUE(pkg->issue_tracker().issues().empty());

  // Generate IR:
  std::unique_ptr<ir::Program> program =
      lang::ir_builder::IRBuilder::TranslateProgram(pkg, pkg_manager.type_info());
  EXPECT_TRUE(program != nullptr);

  // Interpret IR:
  ir_interpreter::Interpreter interpreter(program.get());
  interpreter.run();

  EXPECT_EQ(GetParam().expected_value, interpreter.exit_code());
}

}  // namespace