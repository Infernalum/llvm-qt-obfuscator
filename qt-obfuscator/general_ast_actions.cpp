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

bool GeneralASTVisitor::VisitFieldDecl(clang::FieldDecl *decl) {
  auto loc{decl->getLocation()};
  if (isValidLocation(loc)) {
    addToList(loc, Enums::Stmts::FieldDecl, decl->getQualifiedNameAsString());
    // Decl->dump();
  }
  return true;
}

bool GeneralASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl *decl) {
  auto loc{decl->getLocation()};
  if (isValidLocation(loc)) {
    addToList(loc, Enums::Stmts::RecordDecl, decl->getQualifiedNameAsString());
    // Decl->dump();
  }
  return true;
}

bool GeneralASTVisitor::VisitVarDecl(clang::VarDecl *decl) {
  auto loc{decl->getLocation()};
  if (isValidLocation(loc)) {
    addToList(loc, Enums::Stmts::VarDecl, decl->getQualifiedNameAsString());
    // Decl->dump();
  }
  return true;
}

bool GeneralASTVisitor::VisitCXXMethodDecl(clang::CXXMethodDecl *decl) {
  auto loc{decl->getLocation()};
  if (isValidLocation(loc)) {
    if (clang::isa<clang::CXXConstructorDecl>(*decl))
      addToList(loc, Enums::Stmts::ConstructorDecl,
                decl->getQualifiedNameAsString());
    else if (clang::isa<clang::CXXConversionDecl>(*decl))
      addToList(loc, Enums::Stmts::ConversionDecl,
                decl->getQualifiedNameAsString());
    else if (clang::isa<clang::CXXDestructorDecl>(*decl))
      addToList(loc, Enums::Stmts::DestructorDecl,
                decl->getQualifiedNameAsString());
    else
      addToList(loc, Enums::Stmts::MethodDecl,
                decl->getQualifiedNameAsString());
  }
  return true;
}

bool GeneralASTVisitor::VisitParmVarDecl(clang::ParmVarDecl *decl) {
  auto loc{decl->getLocation()};
  if (isValidLocation(loc)) {
    // auto body = Decl->getBody();
    // body->dumpColor();
    // body->dump();
    addToList(loc, Enums::Stmts::VarDecl, decl->getQualifiedNameAsString());
  }
  return true;
}

bool GeneralASTVisitor::VisitCallExpr(clang::CallExpr *expr) {
  auto &SM = rewriter_.getSourceMgr();
  auto decl = expr->getDirectCallee();
  if ((decl != nullptr) and (*decl).getNameAsString() == "qmlRegisterType") {
    auto args{expr->arguments()};
    // Во всех перегрузках qmlRegisterType имя qml класса идет последним,
    // поэтому парсим аргументы и ищем последний
    size_t arg_num{0};
    auto last_arg = args.begin();
    for (const auto &arg : args)
      ++arg_num;
    for (int i = 0; i < arg_num; ++i)
      ++last_arg;
    if (clang::isa<clang::ImplicitCastExpr>(*last_arg)) {
      auto string = clang::cast<clang::ImplicitCastExpr>(*last_arg);
    }
  }
  // errs() << (cast<Stmt>(arg)).name
  // errs() << Decl << '\n';
  return true;
}

bool GeneralASTVisitor::VisitFunctionDecl(clang::FunctionDecl *decl) {
  auto loc{decl->getLocation()};
  if (isValidLocation(loc)) {
    // auto body = Decl->getBody();
    // Делегируем обработку методов-членов класса на CXXMethodDecl
    if (clang::isa<clang::CXXMethodDecl>(*decl))
      return true;
    addToList(loc, Enums::Stmts::FunctionDecl,
              decl->getQualifiedNameAsString());
  }
  return true;
}

bool GeneralASTVisitor::VisitNamedDecl(clang::NamedDecl *decl) {
  auto location_{decl->getLocation()};
  // Выводим только узлы из обрабатываемых источников
  if (isValidLocation(location_))
    decl->dump();
  // В остальных случаях объявление нам не интересно
  return true;
}

std::string
GeneralASTVisitor::getDeclLocation(const clang::SourceLocation &loc) const {
  std::ostringstream OSS;
  OSS << source_manager_.getFilename(loc).str() << ":"
      << source_manager_.getSpellingLineNumber(loc) << ":"
      << source_manager_.getSpellingColumnNumber(loc);
  return OSS.str();
}

bool GeneralASTVisitor::isValidLocation(
    const clang::SourceLocation &loc) const {
  auto filename{source_manager_.getFilename(loc).str()};
  for (auto &file : allowedLocs)
    if (filename == file)
      return true;
  return false;
}

bool GeneralASTVisitor::addToList(const clang::SourceLocation &loc,
                                  const Enums::Stmts type,
                                  const std::string &named_decl) const {
  auto filename{source_manager_.getFilename(loc).str()};
  auto it{namedList.find(filename)};
  // Если это первое найденное объявление в текущем файле, то добавить новый
  // элемент в мапу
  if (it == namedList.end()) {
    std::set<OwnPair> newEl{OwnPair{type, named_decl}};
    namedList.insert(make_pair(filename, newEl));
  }
  // Иначе добавить объвяление в существующий элемент. Дубликаты учитываются
  // компоратором структуры OwnPair
  else {
    auto &currentSet = it->second;
    currentSet.insert(OwnPair{type, named_decl});
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

bool createFilenames(const std::set<std::string> &locs) {
  for (const auto &Loc : locs) {
    auto slash = Loc.find_last_of("/\\");
    filenames.insert(Loc.substr(slash + 1));
  }
  return true;
}