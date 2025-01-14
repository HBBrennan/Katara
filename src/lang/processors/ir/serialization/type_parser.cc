//
//  type_parser.cc
//  Katara
//
//  Created by Arne Philipeit on 6/6/22.
//  Copyright © 2022 Arne Philipeit. All rights reserved.
//

#include "type_parser.h"

#include "src/common/logging/logging.h"

namespace lang {
namespace ir_serialization {

using ::common::logging::fail;

const ir::Type* TypeParser::ParseType() {
  if (scanner().token() == ::ir_serialization::Scanner::kIdentifier) {
    std::string name = scanner().token_text();
    if (name == "lshared_ptr") {
      return ParseSharedPointer();
    } else if (name == "lunique_ptr") {
      return ParseUniquePointer();
    } else if (name == "lstr") {
      return ir_ext::string();
    } else if (name == "larray") {
      return ParseArray();
    } else if (name == "lstruct") {
      return ParseStruct();
    } else if (name == "linterface") {
      return ParseInterface();
    } else if (name == "ltypeid") {
      return ir_ext::type_id();
    }
  }
  return ::ir_serialization::TypeParser::ParseType();
}

const ir_ext::SharedPointer* TypeParser::ParseSharedPointer() {
  if (scanner().ConsumeIdentifier() != "lshared_ptr") {
    fail("expected lshared_ptr");
  }
  scanner().ConsumeToken(::ir_serialization::Scanner::kAngleOpen);
  const ir::Type* element = ParseType();
  scanner().ConsumeToken(::ir_serialization::Scanner::kComma);

  std::string c = scanner().ConsumeIdentifier().value_or("s");
  if (c != "s" && c != "w") {
    fail("expected 's' or 'w'");
  }
  bool is_strong = c == "s";
  scanner().ConsumeToken(::ir_serialization::Scanner::kAngleClose);

  auto pointer = std::make_unique<ir_ext::SharedPointer>(is_strong, element);
  const ir_ext::SharedPointer* pointer_ptr = pointer.get();
  program()->type_table().AddType(std::move(pointer));
  return pointer_ptr;
}

const ir_ext::UniquePointer* TypeParser::ParseUniquePointer() {
  if (scanner().ConsumeIdentifier() != "lunique_ptr") {
    fail("expected lunique_ptr");
  }
  scanner().ConsumeToken(::ir_serialization::Scanner::kAngleOpen);
  const ir::Type* element = ParseType();
  scanner().ConsumeToken(::ir_serialization::Scanner::kAngleClose);

  auto pointer = std::make_unique<ir_ext::UniquePointer>(element);
  const ir_ext::UniquePointer* pointer_ptr = pointer.get();
  program()->type_table().AddType(std::move(pointer));
  return pointer_ptr;
}

const ir_ext::Array* TypeParser::ParseArray() {
  if (scanner().ConsumeIdentifier() != "larray") {
    fail("expected larray");
  }
  ir_ext::ArrayBuilder builder;
  scanner().ConsumeToken(::ir_serialization::Scanner::kAngleOpen);
  builder.SetElement(ParseType());
  if (scanner().token() == ::ir_serialization::Scanner::kComma) {
    scanner().ConsumeToken(::ir_serialization::Scanner::kComma);
    builder.SetFixedCount(scanner().ConsumeInt64().value_or(0));
  }
  scanner().ConsumeToken(::ir_serialization::Scanner::kAngleClose);

  const ir_ext::Array* array_ptr = builder.Get();
  program()->type_table().AddType(builder.Build());
  return array_ptr;
}

const ir_ext::Struct* TypeParser::ParseStruct() {
  if (scanner().ConsumeIdentifier() != "lstruct") {
    fail("expected lstruct");
  }
  if (scanner().token() != ::ir_serialization::Scanner::kAngleOpen) {
    return ir_ext::empty_struct();
  }
  ir_ext::StructBuilder builder;
  scanner().ConsumeToken(::ir_serialization::Scanner::kAngleOpen);
  ParseStructField(builder);
  while (scanner().token() == ::ir_serialization::Scanner::kComma) {
    scanner().ConsumeToken(::ir_serialization::Scanner::kComma);
    ParseStructField(builder);
  }
  scanner().ConsumeToken(::ir_serialization::Scanner::kAngleClose);

  const ir_ext::Struct* struct_ptr = builder.Get();
  program()->type_table().AddType(builder.Build());
  return struct_ptr;
}

void TypeParser::ParseStructField(ir_ext::StructBuilder& builder) {
  if (scanner().token() != ::ir_serialization::Scanner::kIdentifier) {
    scanner().AddErrorForUnexpectedToken({::ir_serialization::Scanner::kIdentifier});
    scanner().SkipPastTokenSequence({::ir_serialization::Scanner::kComma});
    return;
  }
  std::string name = scanner().ConsumeIdentifier().value();
  scanner().ConsumeToken(::ir_serialization::Scanner::kColon);
  const ir::Type* type = ParseType();
  builder.AddField(name, type);
}

const ir_ext::Interface* TypeParser::ParseInterface() {
  if (scanner().ConsumeIdentifier() != "linterface") {
    fail("expected linterface");
  }
  if (scanner().token() != ::ir_serialization::Scanner::kAngleOpen) {
    return ir_ext::empty_interface();
  }
  ir_ext::InterfaceBuilder builder;
  scanner().ConsumeToken(::ir_serialization::Scanner::kAngleOpen);
  ParseInterfaceMethod(builder);
  while (scanner().token() == ::ir_serialization::Scanner::kComma) {
    scanner().ConsumeToken(::ir_serialization::Scanner::kComma);
    ParseInterfaceMethod(builder);
  }
  scanner().ConsumeToken(::ir_serialization::Scanner::kAngleClose);

  const ir_ext::Interface* interface_ptr = builder.Get();
  program()->type_table().AddType(builder.Build());
  return interface_ptr;
}

void TypeParser::ParseInterfaceMethod(ir_ext::InterfaceBuilder& builder) {
  if (scanner().token() != ::ir_serialization::Scanner::kIdentifier) {
    scanner().AddErrorForUnexpectedToken({::ir_serialization::Scanner::kIdentifier});
    scanner().SkipPastTokenSequence({::ir_serialization::Scanner::kComma});
    return;
  }
  std::string name = scanner().ConsumeIdentifier().value();
  scanner().ConsumeToken(::ir_serialization::Scanner::kColon);
  scanner().ConsumeToken(::ir_serialization::Scanner::kParenOpen);
  std::vector<const ir::Type*> parameters;
  if (scanner().token() != ::ir_serialization::Scanner::kParenClose) {
    parameters = ParseTypes();
  }
  scanner().ConsumeToken(::ir_serialization::Scanner::kParenClose);
  scanner().ConsumeToken(::ir_serialization::Scanner::kArrow);
  scanner().ConsumeToken(::ir_serialization::Scanner::kParenOpen);
  std::vector<const ir::Type*> results;
  if (scanner().token() != ::ir_serialization::Scanner::kParenClose) {
    results = ParseTypes();
  }
  scanner().ConsumeToken(::ir_serialization::Scanner::kParenClose);
  builder.AddMethod(name, parameters, results);
}

}  // namespace ir_serialization
}  // namespace lang
