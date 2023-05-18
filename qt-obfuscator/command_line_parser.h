#ifndef COMMAND_LINE_PARSER_H_INCLUDED_
#define COMMAND_LINE_PARSER_H_INCLUDED_

#include "constants.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/ReplacementsYaml.h"

// Для тривиальной записи изменений в yaml-файл
struct RenameAllInfo {
  size_t offset_;
  std::string qualified_name_;
  std::string new_name_;
};
LLVM_YAML_IS_SEQUENCE_VECTOR(RenameAllInfo)

namespace llvm {
namespace yaml {

template <> struct MappingTraits<RenameAllInfo> {
  static void mapping(IO &IO, RenameAllInfo &Info) {
    IO.mapOptional("Offset", Info.offset_);
    IO.mapOptional("QualifiedName", Info.qualified_name_);
    IO.mapOptional("NewName", Info.new_name_);
  }
};

} // namespace yaml
} // namespace llvm

namespace frontendNS {

// Класс парсера командной строки. ОБрабатывает все кастомные команды и команды
// препроцессору (like --extra-arg=) и следит за валидностью параметров
class Frontend {
public:
  static llvm::cl::OptionCategory tool_category_;

  static llvm::cl::opt<std::string> qtpath_;
  static llvm::cl::alias qtpath_alias_;

  static llvm::cl::list<std::string> qmlfiles_;

  static llvm::cl::opt<bool> inplace_;
  static llvm::cl::alias inplace_alias_;

  static llvm::cl::opt<std::string> export_fixes_;
  static llvm::cl::alias export_fixes_alias_;

  static llvm::cl::opt<Enums::OptEncryprtion> encryption_mode_;
  static llvm::cl::alias encryption_mode_alias_;

  static llvm::cl::opt<int> rng_seed_;

  static llvm::Expected<clang::tooling::CommonOptionsParser>
  create(int argc, const char **argv, llvm::StringRef overwiew = "");

protected:
  static llvm::Expected<bool> isValidOptions();
};

} // namespace frontendNS

#endif /* COMMAND_LINE_PARSER_H_INCLUDED_ */