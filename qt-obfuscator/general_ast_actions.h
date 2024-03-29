#ifndef GENERAL_AST_ACTIONS_H_INCLUDED_
#define GENERAL_AST_ACTIONS_H_INCLUDED_

#include "constants.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Tooling.h"

#include <iostream>
#include <sstream>

class FindQMLMatcher : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  FindQMLMatcher(){};
  virtual void
  run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;

  const clang::ast_matchers::StatementMatcher &getQmlMatcher() const {
    return qml_matcher_;
  };

private:
  static clang::ast_matchers::StatementMatcher qml_matcher_;
};

class GeneralASTVisitor : public clang::RecursiveASTVisitor<GeneralASTVisitor> {
public:
  GeneralASTVisitor(clang::Rewriter &rewriter)
      : rewriter_{rewriter}, source_manager_{rewriter.getSourceMgr()} {};

  bool dataTraverseStmtPre(clang::Stmt *stmt);

  bool dataTraverseStmtPost(clang::Stmt *stmt);

  // RecursiveASTVisitor выступает аналогом Наблюдателя: макрос Visit##(##
  // *Decl) позволяет переопределить этот метод для любого вида узла, и он будет
  // автоматически вызываться, когда возвожен каст рассматриваемого узла к
  // данному типу. Причем, если находится, например, NamedDecl (потомок Decl),
  // то сначала будет вызываться VisitDecl, а потом уже VisitNamedDecl

  bool VisitFieldDecl(clang::FieldDecl *decl);

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *decl);

  bool VisitVarDecl(clang::VarDecl *decl);

  bool VisitCXXMethodDecl(clang::CXXMethodDecl *decl);

  bool VisitParmVarDecl(clang::ParmVarDecl *decl);

  bool VisitCallExpr(clang::CallExpr *expr);

  bool VisitFunctionDecl(clang::FunctionDecl *decl);

  // Так как нас интересуются только использование имен в коде, а NamedDecl
  // является базовым для остальных классов, то здесь можно вызвать dump(),
  // чтобы печатать только те узлы, что входят в обрабатываемые файлы
  bool VisitNamedDecl(clang::NamedDecl *decl);

private:
  // Класс Rewriter позволяет сопоставлять узлы дерева (которое хранится во
  // время программы в ram) с их физическим расположением, позволяя тем самым
  // работать сразу с токенами файлов, а не AST
  clang::Rewriter &rewriter_;
  clang::SourceManager &source_manager_;

  std::string getDeclLocation(const clang::SourceLocation &loc) const;

  // Сверяется с SourceManager и проверяет, что узел является частью
  // обрабатываемых файлов
  bool isValidLocation(const clang::SourceLocation &loc) const;

  // Добавляет найденное объявление в глобальный список namedList
  bool addToList(const clang::SourceLocation &loc, const Enums::Stmts type,
                 const std::string &named_decl) const;
};

// Реализация интерфейса для чтения AST-дерева, созданного препроцессором clang
class GeneralASTConsumer : public clang::ASTConsumer {
public:
  GeneralASTConsumer(clang::Rewriter &rewriter)
      : rewriter_{rewriter}, visitor_{rewriter} {};

  // Переопределение метода, который будет вызываться каждый раз при обработке
  // верхнего узла дерева (т.е. при инициализации дерева для каждого файла)
  bool HandleTopLevelDecl(clang::DeclGroupRef DR) final;

  void PrintStats() final { std::cout << "just for debug\n"; }

private:
  clang::Rewriter &rewriter_;
  GeneralASTVisitor visitor_;
};

// Для каждого указанного исходника создается свое представление AST в памяти
class GeneralASTAction : public clang::ASTFrontendAction {
public:
  GeneralASTAction(){};

  virtual void EndSourceFileAction() final;

  // Чисто виртуальный метод для создания указателя на созданное дерево
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef file) final;

private:
  clang::Rewriter rewriter_;
};

bool createLocs(const std::vector<std::string> &sources);

bool createFilenames(const std::set<std::string> &locs);

#endif /* GENERAL_AST_ACTIONS_H_INCLUDED_ */