#include "general_ast_actions.h"

std::set<std::string> allowedLocs;
std::set<std::string> filenames;
std::map<std::string, std::set<OwnPair>> namedList;

clang::ast_matchers::StatementMatcher FindQMLMatcher::qml_matcher_ =
    clang::ast_matchers::callExpr(
        clang::ast_matchers::callee(clang::ast_matchers::functionDecl(
            clang::ast_matchers::hasName("qmlRegisterType"))))
        .bind("qmlRegisterTypes");

void FindQMLMatcher::run(
    const clang::ast_matchers::MatchFinder::MatchResult &Result) {
  if (const auto FS = Result.Nodes.getNodeAs<clang::Stmt>("qmlRegisterTypes"))
    FS->dump();
}

bool GeneralASTVisitor::dataTraverseStmtPre(clang::Stmt *S) { return true; }

bool GeneralASTVisitor::dataTraverseStmtPost(clang::Stmt *S) { return true; }

bool GeneralASTVisitor::VisitFieldDecl(clang::FieldDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    addToList(Loc, Enums::Stmts::FieldDecl, Decl->getQualifiedNameAsString());
    // Decl->dump();
  }
  return true;
}

bool GeneralASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    addToList(Loc, Enums::Stmts::RecordDecl, Decl->getQualifiedNameAsString());
    // Decl->dump();
  }
  return true;
}

bool GeneralASTVisitor::VisitVarDecl(clang::VarDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    addToList(Loc, Enums::Stmts::VarDecl, Decl->getQualifiedNameAsString());
    // Decl->dump();
  }
  return true;
}

bool GeneralASTVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    if (clang::isa<clang::CXXConstructorDecl>(*Decl))
      addToList(Loc, Enums::Stmts::ConstructorDecl,
                Decl->getQualifiedNameAsString());
    else if (clang::isa<clang::CXXConversionDecl>(*Decl))
      addToList(Loc, Enums::Stmts::ConversionDecl,
                Decl->getQualifiedNameAsString());
    else if (clang::isa<clang::CXXDestructorDecl>(*Decl))
      addToList(Loc, Enums::Stmts::DestructorDecl,
                Decl->getQualifiedNameAsString());
    else
      addToList(Loc, Enums::Stmts::MethodDecl,
                Decl->getQualifiedNameAsString());
  }
  return true;
}

bool GeneralASTVisitor::VisitParmVarDecl(clang::ParmVarDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    // auto body = Decl->getBody();
    // body->dumpColor();
    // body->dump();
    addToList(Loc, Enums::Stmts::VarDecl, Decl->getQualifiedNameAsString());
  }
  return true;
}

bool GeneralASTVisitor::VisitCallExpr(clang::CallExpr *Expr) {
  auto &SM = rewriter_.getSourceMgr();
  auto Decl = Expr->getDirectCallee();
  if ((Decl != nullptr) and (*Decl).getNameAsString() == "qmlRegisterType") {
    auto args{Expr->arguments()};
    // Во всех перегрузках qmlRegisterType имя qml класса идет последним,
    // поэтому парсим аргументы и ищем последний
    size_t arg_num{0};
    auto lastArg = args.begin();
    for (const auto &arg : args)
      ++arg_num;
    for (int i = 0; i < arg_num; ++i)
      ++lastArg;
    if (clang::isa<clang::ImplicitCastExpr>(*lastArg)) {
      auto string = clang::cast<clang::ImplicitCastExpr>(*lastArg);
    }
  }
  // errs() << (cast<Stmt>(arg)).name
  // errs() << Decl << '\n';
  return true;
}

bool GeneralASTVisitor::VisitFunctionDecl(clang::FunctionDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    // auto body = Decl->getBody();
    // Делегируем обработку методов-членов класса на CXXMethodDecl
    if (clang::isa<clang::CXXMethodDecl>(*Decl))
      return true;
    addToList(Loc, Enums::Stmts::FunctionDecl,
              Decl->getQualifiedNameAsString());
  }
  return true;
}

bool GeneralASTVisitor::VisitNamedDecl(clang::NamedDecl *Decl) {
  auto location_{Decl->getLocation()};
  // Выводим только узлы из обрабатываемых источников
  if (isValidLocation(location_)) {
    Decl->dump();
  }
  // В остальных случаях объявление нам не интересно
  return true;
}

std::string
GeneralASTVisitor::getDeclLocation(const clang::SourceLocation &Loc) const {
  std::ostringstream OSS;
  OSS << source_manager_.getFilename(Loc).str() << ":"
      << source_manager_.getSpellingLineNumber(Loc) << ":"
      << source_manager_.getSpellingColumnNumber(Loc);
  return OSS.str();
}

bool GeneralASTVisitor::isValidLocation(
    const clang::SourceLocation &Loc) const {
  auto filename{source_manager_.getFilename(Loc).str()};
  for (auto &file : allowedLocs)
    if (filename == file)
      return true;
  return false;
}

bool GeneralASTVisitor::addToList(const clang::SourceLocation &Loc,
                                  const Enums::Stmts type,
                                  const std::string &NamedDecl) const {
  auto filename{source_manager_.getFilename(Loc).str()};
  auto it{namedList.find(filename)};
  // Если это первое найденное объявление в текущем файле, то добавить новый
  // элемент в мапу
  if (it == namedList.end()) {
    std::set<OwnPair> newEl{OwnPair{type, NamedDecl}};
    namedList.insert(make_pair(filename, newEl));
  }
  // Иначе добавить объвяление в существующий элемент. Дубликаты учитываются
  // компоратором структуры OwnPair
  else {
    auto &currentSet = it->second;
    currentSet.insert(OwnPair{type, NamedDecl});
  }
  return true;
}

bool GeneralASTConsumer::HandleTopLevelDecl(clang::DeclGroupRef DR) {
  for (auto b = DR.begin(), e = DR.end(); b != e; ++b) {
    // Запускаем рекурсивный обход дерева
    visitor_.TraverseDecl(*b);
    //(*b)->dumpColor();
  }
  return true;
};

void GeneralASTAction::EndSourceFileAction() {
  clang::SourceManager &SM = rewriter_.getSourceMgr();
  clang::StringRef filename =
      SM.getFileEntryForID(SM.getMainFileID())->getName();
  llvm::errs() << " ** EndSourceFileAction for: " << filename << " **\n";
}

std::unique_ptr<clang::ASTConsumer>
GeneralASTAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                    clang::StringRef file) {
  llvm::errs() << " ** Creating AST consumer for: " << file << " **\n";
  rewriter_.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  auto p = std::make_unique<GeneralASTConsumer>(rewriter_);
  return p;
}

bool createLocs(const std::vector<std::string> &sources) {
  clang::FileSystemOptions options{"/"};
  clang::FileManager FM{options};
  for (const auto &source : sources) {
    allowedLocs.insert(source);
    auto header = source.substr(0, source.size() - 4) + ".h";
    // Проверяем существование такого хэдера
    auto file = FM.getFileRef(header, false, false);
    if (auto E = file.takeError())
      continue;
    allowedLocs.insert(header);
  }
  return true;
}

bool createFilenames(const std::set<std::string> &Locs) {
  for (const auto &Loc : Locs) {
    auto slash = Loc.find_last_of("/\\");
    filenames.insert(Loc.substr(slash + 1));
  }
  return true;
}