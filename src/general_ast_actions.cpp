#include "general_ast_actions.h"

set<string> allowedLocs;
set<string> filenames;
map<string, set<OwnPair>> namedList;

StatementMatcher FindQMLMatcher::qml_matcher_ =
    callExpr(callee(functionDecl(hasName("qmlRegisterType"))))
        .bind("qmlRegisterTypes");

void FindQMLMatcher::run(const MatchFinder::MatchResult &Result) {
  if (const auto FS = Result.Nodes.getNodeAs<Stmt>("qmlRegisterTypes"))
    FS->dump();
}

bool GeneralASTVisitor::dataTraverseStmtPre(Stmt *S) { return true; }

bool GeneralASTVisitor::dataTraverseStmtPost(Stmt *S) { return true; }

bool GeneralASTVisitor::VisitFieldDecl(FieldDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    addToList(Loc, Enums::Stmts::FieldDecl, Decl->getQualifiedNameAsString());
    // Decl->dump();
  }
  return true;
}

bool GeneralASTVisitor::VisitCXXRecordDecl(CXXRecordDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    addToList(Loc, Enums::Stmts::RecordDecl, Decl->getQualifiedNameAsString());
    // Decl->dump();
  }
  return true;
}

bool GeneralASTVisitor::VisitVarDecl(VarDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    addToList(Loc, Enums::Stmts::VarDecl, Decl->getQualifiedNameAsString());
    // Decl->dump();
  }
  return true;
}

bool GeneralASTVisitor::VisitCXXMethodDecl(CXXMethodDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    if (isa<CXXConstructorDecl>(*Decl))
      addToList(Loc, Enums::Stmts::ConstructorDecl,
                Decl->getQualifiedNameAsString());
    else if (isa<CXXConversionDecl>(*Decl))
      addToList(Loc, Enums::Stmts::ConversionDecl,
                Decl->getQualifiedNameAsString());
    else if (isa<CXXDestructorDecl>(*Decl))
      addToList(Loc, Enums::Stmts::DestructorDecl,
                Decl->getQualifiedNameAsString());
    else
      addToList(Loc, Enums::Stmts::MethodDecl,
                Decl->getQualifiedNameAsString());
  }
  return true;
}

bool GeneralASTVisitor::VisitParmVarDecl(ParmVarDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    // auto body = Decl->getBody();
    // body->dumpColor();
    // body->dump();
    addToList(Loc, Enums::Stmts::VarDecl, Decl->getQualifiedNameAsString());
  }
  return true;
}

bool GeneralASTVisitor::VisitCallExpr(CallExpr *Expr) {
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
    if (isa<clang::ImplicitCastExpr>(*lastArg)) {
      auto string = cast<clang::ImplicitCastExpr>(*lastArg);
    }
  }
  // errs() << (cast<Stmt>(arg)).name
  // errs() << Decl << '\n';
  return true;
}

bool GeneralASTVisitor::VisitFunctionDecl(FunctionDecl *Decl) {
  auto Loc{Decl->getLocation()};
  if (isValidLocation(Loc)) {
    // auto body = Decl->getBody();
    // Делегируем обработку методов-членов класса на CXXMethodDecl
    if (isa<clang::CXXMethodDecl>(*Decl))
      return true;
    addToList(Loc, Enums::Stmts::FunctionDecl,
              Decl->getQualifiedNameAsString());
  }
  return true;
}

bool GeneralASTVisitor::VisitNamedDecl(NamedDecl *Decl) {
  auto location_{Decl->getLocation()};
  // Выводим только узлы из обрабатываемых источников
  if (isValidLocation(location_)) {
    Decl->dump();
  }
  // В остальных случаях объявление нам не интересно
  return true;
}

string GeneralASTVisitor::getDeclLocation(const SourceLocation &Loc) const {
  ostringstream OSS;
  OSS << source_manager_.getFilename(Loc).str() << ":"
      << source_manager_.getSpellingLineNumber(Loc) << ":"
      << source_manager_.getSpellingColumnNumber(Loc);
  return OSS.str();
}

bool GeneralASTVisitor::isValidLocation(const SourceLocation &Loc) const {
  auto filename{source_manager_.getFilename(Loc).str()};
  for (auto &file : allowedLocs)
    if (filename == file)
      return true;
  return false;
}

bool GeneralASTVisitor::addToList(const SourceLocation &Loc,
                                  const Enums::Stmts type,
                                  const string &NamedDecl) const {
  auto filename{source_manager_.getFilename(Loc).str()};
  auto it{namedList.find(filename)};
  // Если это первое найденное объявление в текущем файле, то добавить новый
  // элемент в мапу
  if (it == namedList.end()) {
    set<OwnPair> newEl{OwnPair{type, NamedDecl}};
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

bool GeneralASTConsumer::HandleTopLevelDecl(DeclGroupRef DR) {
  for (auto b = DR.begin(), e = DR.end(); b != e; ++b) {
    // Запускаем рекурсивный обход дерева
    visitor_.TraverseDecl(*b);
    //(*b)->dumpColor();
  }
  return true;
};

void GeneralASTAction::EndSourceFileAction() {
  SourceManager &SM = rewriter_.getSourceMgr();
  StringRef filename = SM.getFileEntryForID(SM.getMainFileID())->getName();
  errs() << " ** EndSourceFileAction for: " << filename << " **\n";
}

unique_ptr<ASTConsumer>
GeneralASTAction::CreateASTConsumer(CompilerInstance &CI, StringRef file) {
  errs() << " ** Creating AST consumer for: " << file << " **\n";
  rewriter_.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  auto p = make_unique<GeneralASTConsumer>(rewriter_);
  return p;
}

bool createLocs(const vector<string> &sources) {
  FileSystemOptions options{"/"};
  FileManager FM{options};
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

bool createFilenames(const set<string> &Locs) {
  for (const auto &Loc : Locs) {
    auto slash = Loc.find_last_of("/\\");
    filenames.insert(Loc.substr(slash + 1));
  }
  return true;
}