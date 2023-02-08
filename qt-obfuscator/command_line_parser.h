#ifndef COMMAND_LINE_PARSER_H_INCLUDED_
#define COMMAND_LINE_PARSER_H_INCLUDED_

#include "constants.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/ReplacementsYaml.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/YAMLTraits.h"
#include <iostream>

using namespace std;
using namespace llvm;
using namespace clang;

// Для тривиальной записи изменений в yaml-файл

struct RenameAllInfo {
  size_t offset;
  string qualified_name;
  string new_name;
};
LLVM_YAML_IS_SEQUENCE_VECTOR(RenameAllInfo)

namespace llvm {
namespace yaml {

template <> struct MappingTraits<RenameAllInfo> {
  static void mapping(IO &IO, RenameAllInfo &Info) {
    IO.mapOptional("Offset", Info.offset);
    IO.mapOptional("QualifiedName", Info.qualified_name);
    IO.mapOptional("NewName", Info.new_name);
  }
};

} // namespace yaml
} // namespace llvm

namespace frontendNS {

// Класс парсера командной строки. ОБрабатывает все кастомные команды и команды
// препроцессору (like --extra-arg=) и следит за валидностью параметров
class Frontend {

public:
  static cl::OptionCategory toolCategory;

  static cl::opt<string> qtPath;
  static cl::alias qtPathAlias;

  static cl::list<string> qmlFiles;

  static cl::opt<bool> inplace;
  static cl::alias inplaceAlias;

  static cl::opt<string> exportFixes;
  static cl::alias exportFixesAlias;

  static cl::opt<Enums::OptEncryprtion> encryptionMode;
  static cl::alias encryptionModeAlias;

  static cl::opt<int> RNGSeed;

  static Expected<tooling::CommonOptionsParser>
  create(int argc, const char **argv, StringRef overwiew = "");

protected:
  static Expected<bool> isValidOptions();
};

} // namespace frontendNS

#endif /* COMMAND_LINE_PARSER_H_INCLUDED_ */