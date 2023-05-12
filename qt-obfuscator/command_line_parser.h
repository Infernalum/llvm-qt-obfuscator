#ifndef COMMAND_LINE_PARSER_H_INCLUDED_
#define COMMAND_LINE_PARSER_H_INCLUDED_

#include "constants.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/ReplacementsYaml.h"

// Для тривиальной записи изменений в yaml-файл
struct RenameAllInfo {
  size_t m_offset;
  std::string m_qualified_name;
  std::string m_new_name;
};
LLVM_YAML_IS_SEQUENCE_VECTOR(RenameAllInfo)

namespace llvm {
namespace yaml {

template <> struct MappingTraits<RenameAllInfo> {
  static void mapping(IO &IO, RenameAllInfo &Info) {
    IO.mapOptional("Offset", Info.m_offset);
    IO.mapOptional("QualifiedName", Info.m_qualified_name);
    IO.mapOptional("NewName", Info.m_new_name);
  }
};

} // namespace yaml
} // namespace llvm

namespace frontendNS {

// Класс парсера командной строки. ОБрабатывает все кастомные команды и команды
// препроцессору (like --extra-arg=) и следит за валидностью параметров
class Frontend {
public:
  static llvm::cl::OptionCategory toolCategory;

  static llvm::cl::opt<std::string> qtPath;
  static llvm::cl::alias qtPathAlias;

  static llvm::cl::list<std::string> qmlFiles;

  static llvm::cl::opt<bool> inplace;
  static llvm::cl::alias inplaceAlias;

  static llvm::cl::opt<std::string> exportFixes;
  static llvm::cl::alias exportFixesAlias;

  static llvm::cl::opt<Enums::OptEncryprtion> encryptionMode;
  static llvm::cl::alias encryptionModeAlias;

  static llvm::cl::opt<int> RNGSeed;

  static llvm::Expected<clang::tooling::CommonOptionsParser>
  create(int argc, const char **argv, llvm::StringRef overwiew = "");

protected:
  static llvm::Expected<bool> isValidOptions();
};

} // namespace frontendNS

#endif /* COMMAND_LINE_PARSER_H_INCLUDED_ */