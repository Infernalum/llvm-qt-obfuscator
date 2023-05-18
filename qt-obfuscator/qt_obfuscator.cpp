#include "general_ast_actions.h"
#include "pr_generator.h"
#include "qml_refactor.h"

#include "clang/Tooling/Refactoring/Rename/RenamingAction.h"
#include "clang/Tooling/Refactoring/Rename/USRFindingAction.h"

int main(int argc, const char **argv) {

  auto expected_parser =
      frontendNS::Frontend::create(argc, argv, frontendNS::Opts::kGeneralInfo);

  if (auto E = expected_parser.takeError()) {
    llvm::errs() << E << '\n';
    return 1;
  }

  // Инициализируем ГПСЧ
  if (!frontendNS::Frontend::rng_seed_) {
    srand(time(nullptr));
    frontendNS::Frontend::rng_seed_ = rand();
  } else
    srand(frontendNS::Frontend::rng_seed_);
  llvm::errs() << "Generation seed is: " << frontendNS::Frontend::rng_seed_
               << '\n';

  // Создаем начальную утилиту с настройками препроцессора
  auto &options{expected_parser.get()};
  auto &sources{options.getSourcePathList()};
  clang::tooling::RefactoringTool Tool(options.getCompilations(), sources);
  size_t rc{0};
  createLocs(sources);
  createFilenames(allowedLocs);

  // Формиируем в утилите все AST деревья и ищем все объявления
  rc = Tool.run(
      clang::tooling::newFrontendActionFactory<GeneralASTAction>().get());
  if (rc)
    return rc;

  // Удаляем из списка инициализаторы класса
  // (конструкторы/операторы/деструкторы)
  std::set<OwnPair> unique_names{};
  for (const auto &file : namedList) {
    auto &list{file.second};
    for (auto &name : list) {
      const auto &first{name.pair_.first};
      const auto &second{name.pair_.second};
      if (first == Stmts::ConstructorDecl or first == Stmts::DestructorDecl)
        continue;
      // Точку входа тоже удаляем
      if (first == Stmts::FunctionDecl and second == "main")
        continue;
      unique_names.insert(name);
    }
  }

  // For debug
  for (const auto &name : unique_names)
    llvm::errs() << kStmtsMap[name.pair_.first] << " - " << name.pair_.second
                 << '\n';

  // Ищем элементы qml
  FindQMLMatcher matcher;
  clang::ast_matchers::MatchFinder finder;
  finder.addMatcher(matcher.getQmlMatcher(), &matcher);
  Tool.run(clang::tooling::newFrontendActionFactory(&finder).get());

  // Формируем структуры для USRFindingAction
  static llvm::cl::list<std::string> qualified_names("");
  llvm::cl::list<unsigned> symboloffsets("");

  for (const auto &name : unique_names)
    qualified_names.push_back(name.pair_.second);

  // Запускаем для каждого USRFindingAction и RanaimingAction (последний
  // перезапишет файлы)
  clang::tooling::USRFindingAction finding_action(symboloffsets,
                                                  qualified_names, true);
  Tool.run(clang::tooling::newFrontendActionFactory(&finding_action).get());

  if (finding_action.errorOccurred()) {
    return 1;
  }

  const auto &USR_list{finding_action.getUSRList()};
  const auto &Prev_names{finding_action.getUSRSpellings()};

  llvm::errs() << "\tUSRList:\n\n";
  for (const auto &name : USR_list) {
    for (const auto &part : name)
      llvm::errs() << part << "; ";
    llvm::errs() << '\n';
  }

  llvm::errs() << "\tPrevNames:\n\n";
  for (const auto &name : Prev_names)
    llvm::errs() << name << ";\n";
  std::vector<std::vector<std::string>> revised_USR_list{};
  std::vector<std::string> revised_prev_names{};

  // Сохраняем для RenaimingAction только уникальные идентификаторы
  // Индексация нужна, чтобы вместе с USR сразу модифицировать PrevNames
  for (auto i = 0; i < USR_list.size(); ++i) {
    auto &decl{USR_list[i]};
    if (decl.empty())
      continue;
    // Проверяем, является ли найденный токен частью обрабатываемых файлов
    bool is_included{false};
    if (((decl.begin())->find("@S@") == 2) or
        ((decl.begin())->find("@F@") == 2))
      is_included = true;
    for (const auto &filename : filenames) {
      if ((decl.begin())->find(filename) == 2) {
        is_included = true;
        continue;
      }
    }
    // Если USR переменной находится в исходных файлах, то обрабатываем
    if (is_included) {
      if (revised_USR_list.empty()) {
        revised_USR_list.push_back(decl);
        revised_prev_names.push_back(Prev_names[i]);
      } else {
        bool is_absent{false};
        for (const auto &pattern : revised_USR_list) {
          if (decl == pattern) {
            is_absent = true;
            break;
          }
        }
        if (!is_absent) {
          revised_USR_list.push_back(decl);
          revised_prev_names.push_back(Prev_names[i]);
        }
      }
    }
  }

  llvm::errs() << "\trevisedUSRList, size: " << revised_USR_list.size() << '\n';
  for (const auto &name : revised_USR_list) {
    for (const auto &part : name)
      llvm::errs() << part << "; ";
    llvm::errs() << '\n';
  }

  llvm::errs() << "\trevisedPrevNames. size: " << revised_prev_names.size()
               << '\n';
  for (const auto &name : revised_prev_names)
    llvm::errs() << name << ";\n";

  std::vector<std::string> new_names = PRGenerator::generate(
      revised_prev_names, frontendNS::Frontend::encryption_mode_);

  // Порядок: <Stmts, oldName> - newName
  std::vector<std::pair<OwnPair, std::string>> match_names;

  // Формируем список для QmlRefactor
  for (auto i = 0; i < revised_prev_names.size(); ++i) {
    auto &revised_name{revised_prev_names[i]};
    const auto &USR_list{revised_USR_list[i]};
    std::pair<OwnPair, std::string> el;
    el.second = new_names[i];
    for (const auto &origin_pair : unique_names) {
      auto origin_name{origin_pair.pair_.second};
      auto colon{origin_name.find_last_of(":")};
      if (colon != std::string::npos) {
        origin_name = origin_name.substr(colon + 1);
      }
      if (revised_name == origin_name) {
        auto &p{el.first.pair_};
        p.first = origin_pair.pair_.first;
        p.second = origin_name;
        break;
      }
    }
    match_names.push_back(el);
  }

  // По собранным USR старых имен и сгенерированным именам переименовываем все
  // узлы
  clang::tooling::RenamingAction renaming_action(new_names, revised_prev_names,
                                                 revised_USR_list,
                                                 Tool.getReplacements(), true);
  std::unique_ptr<clang::tooling::FrontendActionFactory> Factory =
      clang::tooling::newFrontendActionFactory(&renaming_action);

  int exit_code;

  exit_code = Tool.runAndSave(Factory.get());

  llvm::errs() << allowedLocs.size() << '\n';

  // Переименовываем qml элементы
  QMLRefactor qml_refactor(allowedLocs, match_names);
  qml_refactor.run();

  // Сохраняем изменения в yaml, если задавали параметр exportFixes
  if (!frontendNS::Frontend::export_fixes_.empty()) {
    std::error_code EC;
    llvm::raw_fd_ostream OS(frontendNS::Frontend::export_fixes_, EC,
                            llvm::sys::fs::OF_None);
    clang::tooling::TranslationUnitReplacements TUR;
    const auto &file_to_replacements{Tool.getReplacements()};
    for (const auto &entry : file_to_replacements)
      TUR.Replacements.insert(TUR.Replacements.end(), entry.second.begin(),
                              entry.second.end());
    llvm::yaml::Output YAML(OS);
    YAML << TUR;
    OS.close();
    llvm::errs()
        << "Replacements have been successfully exported to YAML file.\n";
  }

  return exit_code;
}