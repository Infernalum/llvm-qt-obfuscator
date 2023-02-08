#ifndef CONSTANTS_H_INCLUDED_
#define CONSTANTS_H_INCLUDED_

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

namespace Enums {

enum OptEncryprtion { MD5, PJW, RANDOM };

// Список поддерживаемых на данный момент узлов AST
enum Stmts {
  VarDecl, /* Объявления переменных */
  FunctionDecl, /* Объявления функций. Стоит выше MethodDecl */
  MethodDecl, /* Объявления методов-членов класса  */
  RecordDecl,      /* Объявления классов */
  ConstructorDecl, /* Объявления конструкторов */
  ConversionDecl,  /* Объявления операторов */
  DestructorDecl,  /* Объявления деструкторов */
  FieldDecl /* Объявления полей-членов класса */
};

// Для промежуточной отладки
static map<Stmts, string> StmtsMap{pair(VarDecl, "VarDecl"),
                                   pair(FunctionDecl, "FunctionDecl"),
                                   pair(MethodDecl, "MethodDecl"),
                                   pair(RecordDecl, "RecordDecl"),
                                   pair(ConstructorDecl, "ConstructorDecl"),
                                   pair(ConversionDecl, "ConversionDecl"),
                                   pair(DestructorDecl, "DestructorDecl"),
                                   pair(FieldDecl, "FieldDecl")};

} // namespace Enums

// Соответствие найденного объявления с его типом: first
// - тип объявления (переменная/класс/функция), second - полное имя (включая
// разрешение видимости / принадлежность к объекту (классу/структуре/etc.))
struct OwnPair {
  pair<Enums::Stmts, string> m_pair;

  OwnPair(Enums::Stmts type = Enums::Stmts::VarDecl, const string &name = "")
      : m_pair{type, name} {};

  // Для использования в set
  bool operator<(const OwnPair &op) const {
    if (m_pair.first == op.m_pair.first) {
      if (m_pair.second < op.m_pair.second)
        return true;
      else
        return false;
    } else if (m_pair.first < op.m_pair.first)
      return true;
    else
      return false;
  };
};

namespace frontendNS {

namespace Opts {

static const string generalInfo{"This is a simple qt-obfuscator based LLVM!\n"};

static const string defaultQtPath{"/usr/include/x86_64-linux-gnu/qt5"};

static const string versionOverview{"Qt-obfuscator v.0.1"};

} // namespace Opts

} // namespace frontendNS

namespace RNGOpts {

// ОБщие параметры генерируемых имен
static const size_t MIN_SIZE = 2;

static const size_t MAX_SIZE = 10;

// Для проверки, что найденное в файле имя не явлется частью большего имени,
// e.g. setFile это часть setFilename
static const std::string allowedSymbols{
    "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUV"
    "WXYZ0123456789"};

} // namespace RNGOpts

// Глобальные переменные для всей программы

// Список всех файлов проектов, включая заголовочные, чтобы препроцессор (и qml
// парсер) понял, какие .h файлы можно модифицировать, а какие нет
extern set<string> allowedLocs;

// Соответствующие имена исходных файлов для отбора найденных объявлений в
// USRList
extern set<string> filenames;

// Найденные объявления в исходниках с их типами
extern map<string, set<OwnPair>> namedList;

#endif /* CONSTANTS_H_INCLUDED_ */