//
//  packages.h
//  Katara
//
//  Created by Arne Philipeit on 9/6/20.
//  Copyright © 2020 Arne Philipeit. All rights reserved.
//

#ifndef lang_packages_h
#define lang_packages_h

#include <filesystem>
#include <memory>

#include "lang/positions.h"
#include "lang/ast.h"
#include "lang/ast_util.h"
#include "lang/constant.h"
#include "lang/types.h"
#include "lang/issues.h"
#include "lang/scanner.h"
#include "lang/parser.h"
#include "lang/type_checker.h"

namespace lang {
namespace packages {

class Package {
public:
    std::string name() const;
    std::string path() const;
    
    const std::vector<pos::File *>& pos_files() const;
    const std::vector<std::unique_ptr<ast::File>>& ast_files() const;
    types::Package * types_package() const;
    
    bool has_errors() const;
    bool has_fatal_errors() const;
    const std::vector<issues::Issue>& issues() const;
    
private:
    Package() {}
    
    std::string name_;
    std::string path_;
    
    std::vector<pos::File *> pos_files_;
    std::vector<std::unique_ptr<ast::File>> ast_files_;
    types::Package *types_package_;
    
    std::vector<issues::Issue> issues_;
    
    friend class PackageManager;
};

class PackageManager {
public:
    PackageManager(std::string stdlib_path);
    
    pos::FileSet * file_set() const;
    types::TypeInfo * type_info() const;
    
    // GetPackage returns the package in the given package directory if is already loaded,
    // otherwise nullptr is returned.
    Package * GetPackage(std::string pkg_dir);
    // LoadPackage loads (if neccesary) and returns the package in the given package
    // directory.
    Package * LoadPackage(std::string pkg_dir);
    std::vector<Package *> Packages() const;
    
private:
    std::filesystem::path FindPackagePath(std::string import, std::filesystem::path import_path);
    std::vector<std::filesystem::path> FindSourceFiles(std::filesystem::path package_path);
    
    std::filesystem::path stdlib_path_;
    std::unique_ptr<pos::FileSet> file_set_;
    std::unique_ptr<types::TypeInfo> type_info_;
    std::unordered_map<std::string, std::unique_ptr<Package>> packages_;
};

}
}

#endif /* lang_packages_h */
