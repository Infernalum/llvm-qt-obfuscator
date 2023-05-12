#ifndef QML_REFACTOR_H_INCLUDED_
#define QML_REFACTOR_H_INCLUDED_

#include "command_line_parser.h"
#include "general_ast_actions.h"

#include <fstream>
#include <regex>
#include <utility>

// Разрешенные в объявлениях символы для проверки границ: чтобы не поменять
// случайно, например, setFile, когда это объявление setFilename
static const std::string kAllowedNameSymbols{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"};

class QMLRefactor {

public:
  QMLRefactor(const std::set<std::string> &locs,
              const std::vector<std::pair<OwnPair, std::string>> &match_names)
      : m_source_locs{locs}, m_match_names{match_names},
        m_qml_files{frontendNS::Frontend::qmlFiles}, m_qml_classes{},
        m_qml_properties{} {};

  void run();

private:
  bool refactorQmls();
  bool refactorSources();

  std::string &findQProperties(const std::string &filename, const size_t &line,
                               std::string &str);
  std::string &findQmlRegisterTypes(const std::string &filename,
                                    const size_t &line, std::string &str);

  std::string renameQMLClasses(const std::string &file) const;
  bool renameOnSignals(const std::string &file) const;
  bool renameProperties(const std::string &file,
                        const std::string &qmlID) const;
  bool renameSlots(const std::string &file, const std::string &qmlID) const;

  const std::set<std::string> &m_source_locs;
  const std::vector<std::pair<OwnPair, std::string>> &m_match_names;
  // Инициализируется по флагу --qml
  const std::vector<std::string> m_qml_files;
  std::vector<std::pair<std::string, std::string>> m_qml_classes;
  std::set<std::pair<OwnPair, std::string>> m_qml_properties;
};

#endif /* QML_REFACTOR_H_INCLUDED_ */