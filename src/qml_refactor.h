#ifndef QML_REFACTOR_H_INCLUDED_
#define QML_REFACTOR_H_INCLUDED_

#include "command_line_parser.h"
#include "general_ast_actions.h"

#include <fstream>
#include <regex>
#include <utility>

using namespace std;

// Разрешенные в объявлениях символы для проверки границ: чтобы не поменять
// случайно, например, setFile, когда это объявление setFilename
static const string allowedNameSymbols{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"};

class QMLRefactor {

public:
  QMLRefactor(const set<string> &Locs,
              const vector<pair<OwnPair, string>> &matchNames)
      : sourceLocs_{Locs}, matchNames_{matchNames},
        qmlFiles_{frontendNS::Frontend::qmlFiles}, qmlClasses_{},
        qmlProperties_{} {};

  void run();

private:
  bool refactor_qmls();
  bool refactor_sources();

  string &find_q_property(const string &filename, const size_t &line,
                          string &str);
  string &find_qmlregistertype(const string &filename, const size_t &line,
                               string &str);

  string rename_qmlClasses(const string &file) const;
  bool rename_onSignals(const string &file) const;
  bool rename_properties(const string &file, const string &qmlID) const;
  bool rename_slots(const string &file, const string &qmlID) const;

  const set<string> &sourceLocs_;
  const vector<pair<OwnPair, string>> &matchNames_;
  // Инициализируется по флагу --qml
  const vector<string> qmlFiles_;
  vector<pair<string, string>> qmlClasses_;
  set<pair<OwnPair, string>> qmlProperties_;
};

#endif /* QML_REFACTOR_H_INCLUDED_ */