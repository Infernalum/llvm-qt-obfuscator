#include "command_line_parser.h"

namespace frontendNS {

llvm::cl::OptionCategory Frontend::toolCategory("Qt-obfuscator common options");

llvm::cl::opt<std::string> Frontend::qtPath(
    "libraries",
    llvm::cl::desc("path to qt sources folder "
                   "(default=/usr/include/x86_64-linux-gnu/qt5)"),
    llvm::cl::value_desc("folder"), llvm::cl::cat(toolCategory),
    llvm::cl::init(Opts::defaultQtPath));
llvm::cl::alias Frontend::qtPathAlias("l",
                                      llvm::cl::desc("alias for --libraries"),
                                      llvm::cl::aliasopt(qtPath),
                                      llvm::cl::cat(toolCategory));

llvm::cl::list<std::string>
    Frontend::qmlFiles("qml", llvm::cl::desc("qml files to be owerwritten"),
                       llvm::cl::value_desc("qml files"),
                       llvm::cl::cat(toolCategory));

llvm::cl::opt<bool> Frontend::inplace(
    "inplace", llvm::cl::desc("Overwrite edited <file>s (default=0)"),
    llvm::cl::value_desc("1 is overwrite, otherwise is create new ones"),
    llvm::cl::cat(Frontend::toolCategory), llvm::cl::init(0));
llvm::cl::alias Frontend::inplaceAlias("i",
                                       llvm::cl::desc("alias for --inplace"),
                                       llvm::cl::aliasopt(inplace),
                                       llvm::cl::cat(toolCategory));

llvm::cl::opt<std::string> Frontend::exportFixes(
    "export-fixes",
    llvm::cl::desc(
        "YAML file to store suggested fixes in (in case of --inplace=1)"),
    llvm::cl::value_desc("filename"), llvm::cl::cat(toolCategory));
llvm::cl::alias
    Frontend::exportFixesAlias("f", llvm::cl::desc("alias for --export-fixes"),
                               llvm::cl::aliasopt(exportFixes),
                               llvm::cl::cat(toolCategory));

llvm::cl::opt<Enums::OptEncryprtion> Frontend::encryptionMode(
    "encryption-mode",
    llvm::cl::desc(
        "Choose encryption mode for literal encoding (default=RANDOM)"),
    llvm::cl::values(
        clEnumValN(Enums::OptEncryprtion::PJW, "PJW", "PJW-32 encryption"),
        clEnumValN(Enums::OptEncryprtion::MD5, "MD5", "MD5 encryption"),
        clEnumValN(Enums::OptEncryprtion::RANDOM, "RAND",
                   "Random declarations creation")),
    llvm::cl::cat(toolCategory));
llvm::cl::alias Frontend::encryptionModeAlias(
    "e", llvm::cl::desc("alias for --encryption-mode"),
    llvm::cl::aliasopt(encryptionMode), llvm::cl::cat(toolCategory));

llvm::cl::opt<int> Frontend::RNGSeed(
    "seed", llvm::cl::desc("Random Number Generator seed"),
    llvm::cl::value_desc("To repeat some results use the same seed"),
    llvm::cl::cat(toolCategory), llvm::cl::init(0));

llvm::Expected<clang::tooling::CommonOptionsParser>
Frontend::create(int argc, const char **argv, llvm::StringRef overwiew) {
  auto expectedParser = llvm::Expected<clang::tooling::CommonOptionsParser>(
      clang::tooling::CommonOptionsParser::create(argc, argv, toolCategory,
                                                  llvm::cl::OneOrMore,
                                                  Opts::generalInfo.c_str()));
  if (!expectedParser)
    return expectedParser.takeError();
  if (auto E = isValidOptions().takeError())
    return std::move(E);
  return expectedParser;
}

llvm::Expected<bool> Frontend::isValidOptions() {
  if (RNGSeed < 0)
    return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                   "Invalid rng-seed: it must be >=0.");
  if (!exportFixes.empty()) {
    std::error_code EC;
    llvm::raw_fd_ostream OS{exportFixes, EC, llvm::sys::fs::OF_None};
    if (EC)
      return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                     "Error YAML file: " + EC.message());
  }

  if (!qmlFiles.empty()) {
    clang::FileSystemOptions options{"/"};
    clang::FileManager FM{options};
    for (const auto &qmlfile : qmlFiles) {
      auto file = FM.getFileRef(qmlfile, false, false);
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