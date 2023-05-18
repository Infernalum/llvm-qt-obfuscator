#include "qml_refactor.h"

extern std::set<std::string> allowedLocs;

void QMLRefactor::run() {
  // for (const auto &pair : _matchNames) {
  //   errs() << pair.first.m_pair.second << " - " << pair.second << " : "
  //          << Enums::StmtsMap[pair.first.m_pair.first] << '\n';
  // }
  refactorSources();
  refactorQmls();
};

bool QMLRefactor::refactorSources() {
  for (const auto &file : source_locs_) {
    std::ifstream opened{file};
    std::string str, buffer;
    size_t line{0};
    while (getline(opened, str)) {
      findQmlRegisterTypes(file, line, str);
      findQProperties(file, line, str);
      ++line;
      buffer += (str + '\n');
    }
    opened.close();
    std::ofstream rewriter(file);
    rewriter << buffer;
    rewriter.close();
  }
  return true;
}

std::string &QMLRefactor::findQProperties(const std::string &filename,
                                          const size_t &line,
                                          std::string &str) {
  auto detect{str.find("Q_PROPERTY(")};
  if (detect != std::string::npos) {
    llvm::errs() << "FOUND Q_PROPERTY macros at " << filename << ":" << line
                 << ":" << detect << '\n';
    // Заменяем все вхождения переименованных идентификаторов на их новые
    // имена
    for (const auto &el : match_names_) {
      const auto &stmt{el.first.pair_.first};
      const auto &old_mame{el.first.pair_.second};
      const auto &new_name{el.second};
      // Проверяем, что найденное совпадение это полное имя идентификатора,  а
      // не часть большего имени
      bool is_border_valid;
      size_t p_str{detect};
      do {
        is_border_valid = true;
        p_str = str.find(old_mame, p_str);
        // Если хотя бы одна граница является допустимым для использования в
        // объявлениях символом, то просто пропускаем совпадение
        if (p_str != std::string::npos) {
          if ((p_str != 0) and (kAllowedNameSymbols.find_first_of(
                                    str[p_str - 1]) != std::string::npos))
            is_border_valid = false;
          if ((p_str != str.size() - 1) and
              (kAllowedNameSymbols.find_first_of(
                   str[p_str + old_mame.size()]) != std::string::npos))
            is_border_valid = false;
          if (is_border_valid) {
            str.replace(p_str, old_mame.size(), new_name);
            qml_properties_.insert(el);
          }
          p_str += old_mame.size();
        }
      } while (p_str != std::string::npos);
    }
  }
  return str;
};

std::string &QMLRefactor::findQmlRegisterTypes(const std::string &filename,
                                               const size_t &line,
                                               std::string &str) {
  auto detect{str.find("qmlRegisterType<")};
  if (detect != std::string::npos) {
    llvm::errs() << "FOUND qmlRegisterType<>() at " << filename << ':' << line
                 << ':' << detect << '\n';
    // Находим последний аргумент функции: во всех перегрузках имя класса
    // идет последним
    auto end{str.find_first_of(';', detect) - 1};
    auto begin{str.find_last_of(',', end) + 1};
    auto className{str.substr(begin, end - begin)};
    className.erase(std::remove(className.begin(), className.end(), ' '),
                    className.end());
    className.erase(std::remove(className.begin(), className.end(), '"'),
                    className.end());

    std::pair<std::string, std::string> pair(className, "");
    for (const auto &el : match_names_) {
      if (className == el.first.pair_.second) {
        pair.second = el.second;
        break;
      }
    }
    pair.second = "QML" + pair.second;
    llvm::errs() << "Renaming " << pair.first << " to " << pair.second
                 << " ...\n";
    str = std::regex_replace(str, std::regex(pair.first), pair.second);
    qml_classes_.push_back(pair);
    llvm::errs() << "Done.\n";
  }
  return str;
};

bool QMLRefactor::refactorQmls() {
  for (const auto &file : qml_files_) {
    auto qmlID = renameQMLClasses(file);
    if (qmlID.size()) {
      renameOnSignals(file);
      renameProperties(file, qmlID);
      renameSlots(file, qmlID);
    }
  }
  return true;
}

std::string QMLRefactor::renameQMLClasses(const std::string &file) const {
  std::string res;
  std::ifstream opened(file);
  std::string buffer, str;
  bool is_declared{false};
  size_t id_loc{std::string::npos};
  while (getline(opened, str)) {
    if (is_declared) {
      id_loc = str.find("id:");
      if (id_loc != std::string::npos) {
        res = str.substr(id_loc + 4, 6);
        is_declared = false;
      }
    }
    for (const auto &qmlclass : qml_classes_) {
      size_t p_str{0};
      p_str = str.find(qmlclass.first);
      if (p_str != std::string::npos) {
        str.replace(p_str, qmlclass.first.size(), qmlclass.second);
        is_declared = true;
      }
    }
    buffer += (str + '\n');
  }
  opened.close();
  std::ofstream rewriter(file);
  rewriter << buffer;
  rewriter.close();
  return res;
};

bool QMLRefactor::renameOnSignals(const std::string &file) const {
  std::ifstream opened(file);
  std::string buffer, str;
  while (getline(opened, str)) {
    for (const auto &el : match_names_) {
      // !!!!!!!!!!!!!!!!!!!
      std::string signal{"on" + el.first.pair_.second};
      signal[2] -= 32;
      auto p_str = str.find(signal);
      if (p_str != std::string::npos) {
        auto newSignal = "on" + el.second;
        if (islower(newSignal[2]))
          newSignal[2] -= 32;
        str.replace(p_str, signal.size(), newSignal);
      }
    }
    buffer += (str + '\n');
  }
  opened.close();
  std::ofstream rewriter(file);
  rewriter << buffer;
  rewriter.close();

  return true;
};

bool QMLRefactor::renameProperties(const std::string &file,
                                   const std::string &qmlID) const {
  std::ifstream opened(file);
  std::string buffer, str;
  auto is_class{false};
  // for (const auto &el : _qmlProperties)
  //   errs() << el.first.m_pair.second << " - " << el.second << '\n';
  while (getline(opened, str)) {
    if (str.find("id: " + qmlID) != std::string::npos) {
      llvm::errs() << str << '\n';
      is_class = true;
    }

    if (is_class)
      for (const auto &el : qml_properties_) {
        auto p_str = str.find(el.first.pair_.second + ':');
        if (p_str != std::string::npos) {
          llvm::errs() << str << '\n';
          str.replace(p_str, el.first.pair_.second.size(), el.second);
          llvm::errs() << str << '\n';
        }
      }

    buffer += (str + '\n');
  }
  opened.close();
  std::ofstream rewriter(file);
  rewriter << buffer;
  rewriter.close();

  return true;
};

bool QMLRefactor::renameSlots(const std::string &file,
                              const std::string &qmlID) const {
  std::ifstream opened(file);
  std::string buffer, str;
  auto call_pattern{qmlID + "."};
  // for (const auto &el : _qmlProperties)
  //   errs() << el.first.m_pair.second << " - " << el.second << '\n';
  size_t line{0};
  while (getline(opened, str)) {
    ++line;
    for (const auto &el : match_names_) {
      auto full_name = call_pattern + el.first.pair_.second;
      auto p_str = str.find(full_name);
      if (p_str != std::string::npos) {
        bool is_border_valid{true};
        // Если хотя бы одна граница является допустимым для использования в
        // объявлениях символом, то просто пропускаем совпадение
        if ((p_str != 0) and (kAllowedNameSymbols.find_first_of(
                                  str[p_str - 1]) != std::string::npos))
          is_border_valid = false;
        if ((p_str != str.size() - 1) and
            (kAllowedNameSymbols.find_first_of(str[p_str + full_name.size()]) !=
             std::string::npos))
          is_border_valid = false;
        if (is_border_valid) {
          llvm::errs() << str << '\n';
          str.replace(p_str, full_name.size(), call_pattern + el.second);
          llvm::errs() << str << '\n';
        }
      }
    }
    buffer += (str + '\n');
  }
  opened.close();
  std::ofstream rewriter(file);
  rewriter << buffer;
  rewriter.close();

  return true;
}