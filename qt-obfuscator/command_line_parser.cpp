#include "command_line_parser.h"

namespace frontendNS {

llvm::cl::OptionCategory
    Frontend::tool_category_("Qt-obfuscator common options");

llvm::cl::opt<std::string> Frontend::qtpath_(
    "libraries",
    llvm::cl::desc("path to qt sources folder "
                   "(default=/usr/include/x86_64-linux-gnu/qt5)"),
    llvm::cl::value_desc("folder"), llvm::cl::cat(tool_category_),
    llvm::cl::init(Opts::kDefaultQtPath));
llvm::cl::alias Frontend::qtpath_alias_("l",
                                        llvm::cl::desc("alias for --libraries"),
                                        llvm::cl::aliasopt(qtpath_),
                                        llvm::cl::cat(tool_category_));

llvm::cl::list<std::string>
    Frontend::qmlfiles_("qml", llvm::cl::desc("qml files to be owerwritten"),
                        llvm::cl::value_desc("qml files"),
                        llvm::cl::cat(tool_category_));

llvm::cl::opt<bool> Frontend::inplace_(
    "inplace", llvm::cl::desc("Overwrite edited <file>s (default=0)"),
    llvm::cl::value_desc("1 is overwrite, otherwise is create new ones"),
    llvm::cl::cat(Frontend::tool_category_), llvm::cl::init(0));
llvm::cl::alias Frontend::inplace_alias_("i",
                                         llvm::cl::desc("alias for --inplace"),
                                         llvm::cl::aliasopt(inplace_),
                                         llvm::cl::cat(tool_category_));

llvm::cl::opt<std::string> Frontend::export_fixes_(
    "export-fixes",
    llvm::cl::desc(
        "YAML file to store suggested fixes in (in case of --inplace=1)"),
    llvm::cl::value_desc("filename"), llvm::cl::cat(tool_category_));
llvm::cl::alias Frontend::export_fixes_alias_(
    "f", llvm::cl::desc("alias for --export-fixes"),
    llvm::cl::aliasopt(export_fixes_), llvm::cl::cat(tool_category_));

llvm::cl::opt<Enums::OptEncryprtion> Frontend::encryption_mode_(
    "encryption-mode",
    llvm::cl::desc(
        "Choose encryption mode for literal encoding (default=RANDOM)"),
    llvm::cl::values(
        clEnumValN(Enums::OptEncryprtion::PJW, "PJW", "PJW-32 encryption"),
        clEnumValN(Enums::OptEncryprtion::MD5, "MD5", "MD5 encryption"),
        clEnumValN(Enums::OptEncryprtion::RANDOM, "RAND",
                   "Random declarations creation")),
    llvm::cl::cat(tool_category_));
llvm::cl::alias Frontend::encryption_mode_alias_(
    "e", llvm::cl::desc("alias for --encryption-mode"),
    llvm::cl::aliasopt(encryption_mode_), llvm::cl::cat(tool_category_));

llvm::cl::opt<int> Frontend::rng_seed_(
    "seed", llvm::cl::desc("Random Number Generator seed"),
    llvm::cl::value_desc("To repeat some results use the same seed"),
    llvm::cl::cat(tool_category_), llvm::cl::init(0));

llvm::Expected<clang::tooling::CommonOptionsParser>
Frontend::create(int argc, const char **argv, llvm::StringRef overwiew) {
  auto expected_parser = llvm::Expected<clang::tooling::CommonOptionsParser>(
      clang::tooling::CommonOptionsParser::create(argc, argv, tool_category_,
                                                  llvm::cl::OneOrMore,
                                                  Opts::kGeneralInfo.c_str()));
  if (!expected_parser)
    return expected_parser.takeError();
  if (auto E = isValidOptions().takeError())
    return std::move(E);
  return expected_parser;
}

llvm::Expected<bool> Frontend::isValidOptions() {
  if (rng_seed_ < 0)
    return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                   "Invalid rng-seed: it must be >=0.");
  if (!export_fixes_.empty()) {
    std::error_code error_code;
    llvm::raw_fd_ostream ostream{export_fixes_, error_code,
                                 llvm::sys::fs::OF_None};
    if (error_code)
      return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                     "Error YAML file: " +
                                         error_code.message());
  }

  if (!qmlfiles_.empty()) {
    clang::FileSystemOptions options{"/"};
    clang::FileManager file_manager{options};
    for (const auto &qmlfile : qmlfiles_) {
      auto file = file_manager.getFileRef(qmlfile, false, false);
      if (auto E = file.takeError()) {
        llvm::errs() << "Error QML file: ";
        return std::move(E);
      }
      if (qmlfile.substr(qmlfile.size() - 4, 4) != ".qml")
        return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                       "Invalid QML file format.");
    }
  }

  return true;
}
} // namespace frontendNS