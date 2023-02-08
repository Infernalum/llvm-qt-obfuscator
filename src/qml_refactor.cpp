#include "qml_refactor.h"

extern set<string> allowedLocs;

void QMLRefactor::run() {
  // for (const auto &pair : _matchNames) {
  //   errs() << pair.first.m_pair.second << " - " << pair.second << " : "
  //          << Enums::StmtsMap[pair.first.m_pair.first] << '\n';
  // }
  refactor_sources();
  refactor_qmls();
};

bool QMLRefactor::refactor_sources() {
  for (const auto &file : sourceLocs_) {
    ifstream opened{file};
    string str;
    string buffer;
    size_t line{0};
    while (getline(opened, str)) {
      find_qmlregistertype(file, line, str);
      find_q_property(file, line, str);
      ++line;
      buffer += (str + '\n');
    }
    opened.close();
    ofstream rewriter(file);
    rewriter << buffer;
    rewriter.close();
  }
  return true;
}

string &QMLRefactor::find_q_property(const string &filename, const size_t &line,
                                     string &str) {
  auto detect{str.find("Q_PROPERTY(")};
  if (detect != string::npos) {
    errs() << "FOUND Q_PROPERTY macros at " << filename << ":" << line << ":"
           << detect << '\n';
    // Заменяем все вхождения переименованных идентификаторов на их новые
    // имена
    for (const auto &el : matchNames_) {
      const auto &Stmt{el.first.m_pair.first};
      const auto &oldName{el.first.m_pair.second};
      const auto &newName{el.second};
      // Проверяем, что найденное совпадение это полное имя идентификатора,  а
      // не часть большего имени
      bool isBorderValid;
      size_t p_str{detect};
      do {
        isBorderValid = true;
        p_str = str.find(oldName, p_str);
        // Если хотя бы одна граница является допустимым для использования в
        // объявлениях символом, то просто пропускаем совпадение
        if (p_str != string::npos) {
          if ((p_str != 0) and (allowedNameSymbols.find_first_of(
                                    str[p_str - 1]) != string::npos))
            isBorderValid = false;
          if ((p_str != str.size() - 1) and
              (allowedNameSymbols.find_first_of(str[p_str + oldName.size()]) !=
               string::npos))
            isBorderValid = false;
          if (isBorderValid) {
            str.replace(p_str, oldName.size(), newName);
            qmlProperties_.insert(el);
          }
          p_str += oldName.size();
        }
      } while (p_str != string::npos);
    }
  }
  return str;
};

string &QMLRefactor::find_qmlregistertype(const string &filename,
                                          const size_t &line, string &str) {
  auto detect{str.find("qmlRegisterType<")};
  if (detect != string::npos) {
    errs() << "FOUND qmlRegisterType<>() at " << filename << ':' << line << ':'
           << detect << '\n';
    // Находим последний аргумент функции: во всех перегрузках имя класса
    // идет последним
    auto e{str.find_first_of(';', detect) - 1};
    auto b{str.find_last_of(',', e) + 1};
    auto className{str.substr(b, e - b)};
    className.erase(std::remove(className.begin(), className.end(), ' '),
                    className.end());
    className.erase(std::remove(className.begin(), className.end(), '"'),
                    className.end());

    pair<string, string> pair(className, "");
    for (const auto &el : matchNames_) {
      if (className == el.first.m_pair.second) {
        pair.second = el.second;
        break;
      }
    }
    pair.second = "QML" + pair.second;
    errs() << "Renaming " << pair.first << " to " << pair.second << " ...\n";
    str = std::regex_replace(str, std::regex(pair.first), pair.second);
    qmlClasses_.push_back(pair);
    errs() << "Done.\n";
  }
  return str;
};

bool QMLRefactor::refactor_qmls() {
  for (const auto &file : qmlFiles_) {
    auto qmlID = rename_qmlClasses(file);
    if (qmlID.size()) {
      rename_onSignals(file);
      rename_properties(file, qmlID);
      rename_slots(file, qmlID);
    }
  }
  return true;
}

string QMLRefactor::rename_qmlClasses(const string &file) const {
  string res;
  ifstream opened(file);
  string buffer;
  string str;
  bool isDeclared{false};
  size_t id_loc{string::npos};
  while (getline(opened, str)) {
    if (isDeclared) {
      id_loc = str.find("id:");
      if (id_loc != string::npos) {
        res = str.substr(id_loc + 4, 6);
        isDeclared = false;
      }
    }
    for (const auto &qmlclass : qmlClasses_) {
      size_t p_str{0};
      p_str = str.find(qmlclass.first);
      if (p_str != string::npos) {
        str.replace(p_str, qmlclass.first.size(), qmlclass.second);
        isDeclared = true;
      }
    }
    buffer += (str + '\n');
  }
  opened.close();
  ofstream rewriter(file);
  rewriter << buffer;
  rewriter.close();
  return res;
};

bool QMLRefactor::rename_onSignals(const string &file) const {
  ifstream opened(file);
  string buffer;
  string str;
  while (getline(opened, str)) {
    for (const auto &el : matchNames_) {
      // !!!!!!!!!!!!!!!!!!!
      string signal{"on" + el.first.m_pair.second};
      signal[2] -= 32;
      auto p_str = str.find(signal);
      if (p_str != string::npos) {
        auto newSignal = "on" + el.second;
        if (islower(newSignal[2]))
          newSignal[2] -= 32;
        str.replace(p_str, signal.size(), newSignal);
      }
    }
    buffer += (str + '\n');
  }
  opened.close();
  ofstream rewriter(file);
  rewriter << buffer;
  rewriter.close();

  return true;
};

bool QMLRefactor::rename_properties(const string &file,
                                    const string &qmlID) const {
  ifstream opened(file);
  string buffer;
  string str;
  auto isClass{false};
  // for (const auto &el : _qmlProperties)
  //   errs() << el.first.m_pair.second << " - " << el.second << '\n';
  while (getline(opened, str)) {
    if (str.find("id: " + qmlID) != string::npos) {
      errs() << str << '\n';
      isClass = true;
    }

    if (isClass)
      for (const auto &el : qmlProperties_) {
        auto p_str = str.find(el.first.m_pair.second + ':');
        if (p_str != string::npos) {
          errs() << str << '\n';
          str.replace(p_str, el.first.m_pair.second.size(), el.second);
          errs() << str << '\n';
        }
      }

    buffer += (str + '\n');
  }
  opened.close();
  ofstream rewriter(file);
  rewriter << buffer;
  rewriter.close();

  return true;
};

bool QMLRefactor::rename_slots(const string &file, const string &qmlID) const {
  ifstream opened(file);
  string buffer;
  string str;
  auto callPattern{qmlID + "."};
  // for (const auto &el : _qmlProperties)
  //   errs() << el.first.m_pair.second << " - " << el.second << '\n';
  size_t line{0};
  while (getline(opened, str)) {
    ++line;
    for (const auto &el : matchNames_) {
      auto fullName = callPattern + el.first.m_pair.second;
      auto p_str = str.find(fullName);
      if (p_str != string::npos) {
        bool isBorderValid{true};
        // Если хотя бы одна граница является допустимым для использования в
        // объявлениях символом, то просто пропускаем совпадение
        if ((p_str != 0) and
            (allowedNameSymbols.find_first_of(str[p_str - 1]) != string::npos))
          isBorderValid = false;
        if ((p_str != str.size() - 1) and
            (allowedNameSymbols.find_first_of(str[p_str + fullName.size()]) !=
             string::npos))
          isBorderValid = false;
        if (isBorderValid) {
          errs() << str << '\n';
          str.replace(p_str, fullName.size(), callPattern + el.second);
          errs() << str << '\n';
        }
      }
    }
    buffer += (str + '\n');
  }
  opened.close();
  ofstream rewriter(file);
  rewriter << buffer;
  rewriter.close();

  return true;
}