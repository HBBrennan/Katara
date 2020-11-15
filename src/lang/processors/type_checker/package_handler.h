//
//  package_handler.h
//  Katara
//
//  Created by Arne Philipeit on 11/8/20.
//  Copyright © 2020 Arne Philipeit. All rights reserved.
//

#ifndef lang_type_checker_package_handler_h
#define lang_type_checker_package_handler_h

#include <memory>
#include <vector>

#include "lang/processors/type_checker/constant_handler.h"
#include "lang/processors/type_checker/type_handler.h"
#include "lang/processors/type_checker/variable_handler.h"

namespace lang {
namespace type_checker {

class PackageHandler {
public:
    static bool ProcessPackage(std::vector<ast::File *> package_files,
                               types::Package *package,
                               types::TypeInfo *type_info,
                               std::vector<issues::Issue>& issues);
    
private:
    class Action {
    public:
        Action(std::unordered_set<types::Object *> prerequisites,
               types::Object *defined_object,
               std::function<bool ()> executor)
        : Action(prerequisites, std::unordered_set<types::Object *>{defined_object}, executor) {}
        
        Action(std::unordered_set<types::Object *> prerequisites,
               std::unordered_set<types::Object *> defined_objects,
               std::function<bool ()> executor)
            : prerequisites_(prerequisites),
              defined_objects_(defined_objects),
              executor_(executor) {}
        
        const std::unordered_set<types::Object *>& prerequisites() const {
            return prerequisites_;
        }
        
        const std::unordered_set<types::Object *>& defined_objects() const {
            return defined_objects_;
        }
        
        bool execute() { return executor_(); }
                
    private:
        std::unordered_set<types::Object *> prerequisites_;
        std::unordered_set<types::Object *> defined_objects_;
        
        std::function<bool ()> executor_;
    };
    
    PackageHandler(std::vector<ast::File *> package_files,
                   types::Package *package,
                   types::TypeInfo *type_info,
                   std::vector<issues::Issue>& issues);
    
    void FindActions();
    void FindActionsForTypeDecl(ast::GenDecl *type_decl);
    void FindActionsForConstDecl(ast::GenDecl *const_decl);
    void FindActionsForVarDecl(ast::GenDecl *var_decl);
    void FindActionsForFuncDecl(ast::FuncDecl *func_decl);
    
    std::unordered_set<types::Object *> FindPrerequisites(ast::Node *node);
    
    std::vector<Action *> FindActionOrder();
    std::vector<Action *>
        FindActionOrderForActions(const std::vector<Action *>& actions,
                                  std::unordered_set<types::Object *>& defined_objects);
    void ReportLoopInActions(const std::vector<Action *>& actions);
    std::unordered_set<types::Object *> FindLoop(const std::vector<Action *>& actions,
                                                 std::vector<Action *> stack);
    
    bool ExecuteActions(std::vector<Action *> ordered_actions);

    std::vector<ast::File *> package_files_;
    types::Package *package_;
    types::TypeInfo *type_info_;
    std::vector<issues::Issue> &issues_;
    
    std::vector<std::unique_ptr<Action>> actions_;
    
    std::vector<Action *> const_and_type_actions_;
    std::vector<Action *> variable_and_func_decl_actions_;
    std::vector<Action *> func_body_actions_;
};

}
}

#endif /* lang_type_checker_package_handler_h */