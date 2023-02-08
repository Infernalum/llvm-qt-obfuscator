#include "general_ast_actions.h"
#include "pr_generator.h"
#include "qml_refactor.h"

#include "clang/Tooling/Refactoring/Rename/RenamingAction.h"
#include "clang/Tooling/Refactoring/Rename/USRFindingAction.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/RandomNumberGenerator.h"

using namespace frontendNS;
using namespace clang;
using namespace llvm;

int main(int argc, const char **argv) {

  auto expectedParser = Frontend::create(argc, argv, Opts::generalInfo);

  if (auto E = expectedParser.takeError()) {
    errs() << E << '\n';
    return 1;
  }

  // Инициализируем ГПСЧ
  if (!Frontend::RNGSeed) {
    srand(time(nullptr));
    Frontend::RNGSeed = rand();
  } else
    srand(Frontend::RNGSeed);
  errs() << "Generation seed is: " << Frontend::RNGSeed << '\n';

  // Создаем начальную утилиту с настройками препроцессора
  auto &options{expectedParser.get()};
  auto &sources{options.getSourcePathList()};
  tooling::RefactoringTool Tool(options.getCompilations(), sources);
  size_t rc{0};
  createLocs(sources);
  createFilenames(allowedLocs);

  // Формиируем в утилите все AST деревья и ищем все объявления
  rc = Tool.run(tooling::newFrontendActionFactory<GeneralASTAction>().get());
  if (rc)
    return rc;

  // Удаляем из списка инициализаторы класса
  // (конструкторы/операторы/деструкторы)
  set<OwnPair> uniqueNames{};
  for (const auto &file : namedList) {
    auto &list{file.second};
    for (auto &name : list) {
      const auto &first{name.m_pair.first};
      const auto &second{name.m_pair.second};
      if (first == Stmts::ConstructorDecl or first == Stmts::DestructorDecl)
        continue;
      // Точку входа тоже удаляем
      if (first == Stmts::FunctionDecl and second == "main")
        continue;
      uniqueNames.insert(name);
    }
  }

  // For debug
  for (const auto &name : uniqueNames)
    errs() << StmtsMap[name.m_pair.first] << " - " << name.m_pair.second
           << '\n';

  // Ищем элементы qml
  FindQMLMatcher matcher;
  MatchFinder finder;
  finder.addMatcher(matcher.getQmlMatcher(), &matcher);
  Tool.run(tooling::newFrontendActionFactory(&finder).get());

  // Формируем структуры для USRFindingAction
  static cl::list<string> qualifiedNames("");
  cl::list<unsigned> symboloffsets("");

  for (const auto &name : uniqueNames)
    qualifiedNames.push_back(name.m_pair.second);

  // Запускаем для каждого USRFindingAction и RanaimingAction (последний
  // перезапишет файлы)
  tooling::USRFindingAction findingAction(symboloffsets, qualifiedNames, true);
  Tool.run(tooling::newFrontendActionFactory(&findingAction).get());

  if (findingAction.errorOccurred()) {
    return 1;
  }

  const auto &USRList{findingAction.getUSRList()};
  const auto &PrevNames{findingAction.getUSRSpellings()};

  errs() << "\tUSRList:\n\n";
  for (const auto &name : USRList) {
    for (const auto &part : name)
      errs() << part << "; ";
    errs() << '\n';
  }

  errs() << "\tPrevNames:\n\n";
  for (const auto &name : PrevNames) {
    errs() << name << ";\n";
  }

  vector<vector<string>> revisedUSRList{};
  vector<string> revisedPrevNames{};

  // Сохраняем для RenaimingAction только уникальные идентификаторы
  // Индексация нужна, чтобы вместе с USR сразу модифицировать PrevNames
  for (auto i = 0; i < USRList.size(); ++i) {
    auto &decl{USRList[i]};
    if (decl.empty())
      continue;
    // Проверяем, является ли найденный токен частью обрабатываемых файлов
    bool isIncluded{false};
    if (((decl.begin())->find("@S@") == 2) or
        ((decl.begin())->find("@F@") == 2))
      isIncluded = true;
    for (const auto &filename : filenames) {
      if ((decl.begin())->find(filename) == 2) {
        isIncluded = true;
        continue;
      }
    }
    // Если USR переменной находится в исходных файлах, то обрабатываем
    if (isIncluded) {
      if (revisedUSRList.empty()) {
        revisedUSRList.push_back(decl);
        revisedPrevNames.push_back(PrevNames[i]);
      } else {
        bool isAbsent{false};
        for (const auto &pattern : revisedUSRList) {
          if (decl == pattern) {
            isAbsent = true;
            break;
          }
        }
        if (!isAbsent) {
          revisedUSRList.push_back(decl);
          revisedPrevNames.push_back(PrevNames[i]);
        }
      }
    }
  }

  errs() << "\trevisedUSRList, size: " << revisedUSRList.size() << '\n';
  for (const auto &name : revisedUSRList) {
    for (const auto &part : name)
      errs() << part << "; ";
    errs() << '\n';
  }

  errs() << "\trevisedPrevNames. size: " << revisedPrevNames.size() << '\n';
  for (const auto &name : revisedPrevNames) {
    errs() << name << ";\n";
  }

  std::vector<std::string> newNames =
      PRGenerator::generate(revisedPrevNames, Frontend::encryptionMode);

  // Порядок: <Stmts, oldName> - newName
  vector<pair<OwnPair, string>> matchNames;

  // Формируем список для QmlRefactor
  for (auto i = 0; i < revisedPrevNames.size(); ++i) {
    auto &revisedName{revisedPrevNames[i]};
    const auto &USRList{revisedUSRList[i]};
    pair<OwnPair, string> el;
    el.second = newNames[i];
    for (const auto &originPair : uniqueNames) {
      auto originName{originPair.m_pair.second};
      auto colon{originName.find_last_of(":")};
      if (colon != string::npos) {
        originName = originName.substr(colon + 1);
      }
      if (revisedName == originName) {
        auto &p{el.first.m_pair};
        p.first = originPair.m_pair.first;
        p.second = originName;
        break;
      }
    }
    matchNames.push_back(el);
  }

  // По собранным USR старых имен и сгенерированным именам переименовываем все
  // узлы
  tooling::RenamingAction RenameAction(
      newNames, revisedPrevNames, revisedUSRList, Tool.getReplacements(), true);
  unique_ptr<tooling::FrontendActionFactory> Factory =
      tooling::newFrontendActionFactory(&RenameAction);

  int ExitCode;

  ExitCode = Tool.runAndSave(Factory.get());

  errs() << allowedLocs.size() << '\n';

  // Переименовываем qml элементы
  QMLRefactor qmlrefactor(allowedLocs, matchNames);
  qmlrefactor.run();

  // Сохраняем изменения в yaml, если задавали параметр exportDixes
  if (!Frontend::exportFixes.empty()) {
    error_code EC;
    raw_fd_ostream OS(Frontend::exportFixes, EC, sys::fs::OF_None);
    tooling::TranslationUnitReplacements TUR;
    const auto &fileToReplacements{Tool.getReplacements()};
    for (const auto &Entry : fileToReplacements)
      TUR.Replacements.insert(TUR.Replacements.end(), Entry.second.begin(),
                              Entry.second.end());
    yaml::Output YAML(OS);
    YAML << TUR;
    OS.close();
    errs() << "Replacements have been successfully exported to YAML file.\n";
  }

  return ExitCode;
}