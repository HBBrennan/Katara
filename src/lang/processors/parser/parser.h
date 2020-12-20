//
//  parser.h
//  Katara
//
//  Created by Arne Philipeit on 5/25/20.
//  Copyright © 2020 Arne Philipeit. All rights reserved.
//

#ifndef lang_parser_h
#define lang_parser_h

#include <memory>
#include <string>
#include <vector>

#include "lang/representation/positions/positions.h"
#include "lang/representation/tokens/tokens.h"
#include "lang/representation/ast/nodes.h"
#include "lang/representation/ast/ast.h"
#include "lang/representation/ast/ast_builder.h"
#include "lang/processors/issues/issues.h"
#include "lang/processors/scanner/scanner.h"

namespace lang {
namespace parser {

class Parser {
public:
    static ast::File * ParseFile(pos::File *file,
                                 ast::ASTBuilder& ast_builder,
                                 std::vector<issues::Issue>& issues);
    
private:
    enum ExprOptions {
        kNoExprOptions = 0,
        kDisallowCompositeLit = 1 << 0,
    };
    enum FuncFieldListOptions {
        kNoFuncFieldListOptions = 0,
        kExpectParen = 1 << 0,
    };
    
    Parser(scanner::Scanner& scanner,
           ast::ASTBuilder& ast_builder,
           std::vector<issues::Issue>& issues)
    : scanner_(scanner), ast_builder_(ast_builder), issues_(issues) {}
    
    ast::File * ParseFile();
    
    ast::Decl * ParseDecl();
    ast::GenDecl * ParseGenDecl();
    ast::Spec * ParseSpec(tokens::Token spec_type);
    ast::ImportSpec * ParseImportSpec();
    ast::ValueSpec * ParseValueSpec();
    ast::TypeSpec * ParseTypeSpec();
    ast::FuncDecl * ParseFuncDecl();
    
    std::vector<ast::Stmt *> ParseStmtList();
    ast::Stmt * ParseStmt();
    ast::Stmt * ParseSimpleStmt(ExprOptions expr_options);
    ast::Stmt *ParseSimpleStmt(ast::Expr * expr, ExprOptions expr_options);
    ast::BlockStmt * ParseBlockStmt();
    ast::DeclStmt * ParseDeclStmt();
    ast::ReturnStmt * ParseReturnStmt();
    ast::IfStmt * ParseIfStmt();
    ast::Stmt * ParseSwitchStmt();
    ast::BlockStmt * ParseSwitchStmtBody();
    ast::CaseClause * ParseCaseClause();
    ast::ForStmt * ParseForStmt();
    ast::BranchStmt * ParseBranchStmt();
    ast::ExprStmt * ParseExprStmt(ast::Expr * x);
    ast::LabeledStmt * ParseLabeledStmt(ast::Ident * label);
    ast::AssignStmt * ParseAssignStmt(ast::Expr * first_expr, ExprOptions expr_options);
    ast::IncDecStmt * ParseIncDecStmt(ast::Expr * x);
    
    std::vector<ast::Expr *> ParseExprList(ExprOptions expr_options);
    std::vector<ast::Expr *> ParseExprList(ast::Expr * first_expr, ExprOptions expr_options);
    ast::Expr * ParseExpr(ExprOptions expr_options);
    ast::Expr * ParseExpr(tokens::precedence_t prec, ExprOptions expr_options);
    ast::Expr * ParseUnaryExpr(ExprOptions expr_options);
    
    ast::Expr * ParsePrimaryExpr(ExprOptions expr_options);
    ast::Expr * ParsePrimaryExpr(ast::Expr * primary_expr, ExprOptions expr_options);
    ast::Expr * ParsePrimaryExpr(ast::Expr * primary_expr,
                                 pos::pos_t l_brack,
                                 std::vector<ast::Expr *> type_args,
                                 pos::pos_t r_brack,
                                 ExprOptions expr_options);
    ast::ParenExpr * ParseParenExpr();
    ast::SelectionExpr * ParseSelectionExpr(ast::Expr * accessed);
    ast::TypeAssertExpr * ParseTypeAssertExpr(ast::Expr * x);
    ast::IndexExpr * ParseIndexExpr(ast::Expr * accessed);
    ast::CallExpr * ParseCallExpr(ast::Expr * func,
                                  pos::pos_t l_brack,
                                  std::vector<ast::Expr *> type_args,
                                  pos::pos_t r_brack);
    ast::FuncLit * ParseFuncLit(ast::FuncType * type);
    ast::CompositeLit * ParseCompositeLit(ast::Expr * type);
    ast::Expr * ParseCompositeLitElement();
    
    static bool CanStartType(tokens::Token token);
    ast::Expr * ParseType();
    ast::Expr * ParseType(ast::Ident * ident);
    ast::ArrayType * ParseArrayType();
    ast::FuncType * ParseFuncType();
    ast::InterfaceType * ParseInterfaceType();
    ast::Expr * ParseEmbdeddedInterface();
    ast::MethodSpec * ParseMethodSpec();
    ast::StructType * ParseStructType();
    ast::UnaryExpr * ParsePointerType();
    ast::TypeInstance * ParseTypeInstance(ast::Expr * type);
    
    ast::ExprReceiver * ParseExprReceiver();
    ast::TypeReceiver * ParseTypeReceiver();
    ast::FieldList * ParseFuncFieldList(FuncFieldListOptions options);
    std::vector<ast::Field *> ParseFuncFields();
    ast::FieldList * ParseStructFieldList();
    ast::Field * ParseStructField();
    ast::TypeParamList * ParseTypeParamList();
    ast::TypeParam * ParseTypeParam();
    
    ast::BasicLit * ParseBasicLit();
    std::vector<ast::Ident *> ParseIdentList(bool split_shift_ops = false);
    ast::Ident * ParseIdent(bool split_shift_ops = false);
    
    scanner::Scanner& scanner_;
    ast::ASTBuilder& ast_builder_;
    std::vector<issues::Issue>& issues_;
};

}
}

#endif /* lang_parser_h */
