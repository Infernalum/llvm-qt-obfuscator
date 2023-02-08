#ifndef GENERAL_AST_ACTIONS_H_INCLUDED_
#define GENERAL_AST_ACTIONS_H_INCLUDED_

#include "constants.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Tooling.h"

#include <sstream>

using namespace std;
using namespace clang;
using namespace llvm;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

class FindQMLMatcher : public MatchFinder::MatchCallback {
public:
  FindQMLMatcher(){};
  virtual void run(const MatchFinder::MatchResult &Result) override;

  const StatementMatcher &getQmlMatcher() const { return qml_matcher_; };

private:
  static StatementMatcher qml_matcher_;
};

class GeneralASTVisitor : public RecursiveASTVisitor<GeneralASTVisitor> {
public:
  GeneralASTVisitor(Rewriter &R)
      : rewriter_{R}, source_manager_{R.getSourceMgr()} {};

  bool dataTraverseStmtPre(Stmt *S);

  bool dataTraverseStmtPost(Stmt *S);

  // RecursiveASTVisitor выступает аналогом Наблюдателя: макрос Visit##(##
  // *Decl) позволяет переопределить этот метод для любого вида узла, и он будет
  // автоматически вызываться, когда возвожен каст рассматриваемого узла к
  // данному типу. Причем, если находится, например, NamedDecl (потомок Decl),
  // то сначала будет вызываться VisitDecl, а потом уже VisitNamedDecl

  bool VisitFieldDecl(FieldDecl *Decl);

  bool VisitCXXRecordDecl(CXXRecordDecl *Decl);

  bool VisitVarDecl(VarDecl *Decl);

  bool VisitCXXMethodDecl(CXXMethodDecl *Decl);

  bool VisitParmVarDecl(ParmVarDecl *Decl);

  bool VisitCallExpr(CallExpr *Expr);

  bool VisitFunctionDecl(FunctionDecl *Decl);

  // Так как нас интересуются только использование имен в коде, а NamedDecl
  // является базовым для остальных классов, то здесь можно вызвать dump(),
  // чтобы печатать только те узлы, что входят в обрабатываемые файлы
  bool VisitNamedDecl(NamedDecl *Decl);

private:
  // Класс Rewriter позволяет сопоставлять узлы дерева (которое хранится во
  // время программы в ram) с их физическим расположением, позволяя тем самым
  // работать сразу с токенами файлов, а не AST
  Rewriter &rewriter_;
  SourceManager &source_manager_;

  string getDeclLocation(const SourceLocation &Loc) const;

  // Сверяется с SourceManager и проверяет, что узел является частью
  // обрабатываемых файлов
  bool isValidLocation(const SourceLocation &Loc) const;

  // Добавляет найденное объявление в глобальный список namedList
  bool addToList(const SourceLocation &Loc, const Enums::Stmts type,
                 const string &NamedDecl) const;
};

// Реализация интерфейса для чтения AST-дерева, созданного препроцессором clang
class GeneralASTConsumer : public ASTConsumer {
public:
  GeneralASTConsumer(Rewriter &R) : rewriter_{R}, visitor_{R} {};

  // Переопределение метода, который будет вызываться каждый раз при обработке
  // верхнего узла дерева (т.е. при инициализации дерева для каждого файла)
  bool HandleTopLevelDecl(DeclGroupRef DR) final;

  void PrintStats() final { cout << "just for debug\n"; }

private:
  Rewriter &rewriter_;
  GeneralASTVisitor visitor_;
};

// Для каждого указанного исходника создается свое представление AST в памяти
class GeneralASTAction : public ASTFrontendAction {
public:
  GeneralASTAction(){};

  virtual void EndSourceFileAction() final;

  // Чисто виртуальный метод для создания указателя на созданное дерево
  virtual unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                    StringRef file) final;

private:
  Rewriter rewriter_;
};

bool createLocs(const vector<string> &sources);

bool createFilenames(const set<string> &Locs);

#endif /* GENERAL_AST_ACTIONS_H_INCLUDED_ */