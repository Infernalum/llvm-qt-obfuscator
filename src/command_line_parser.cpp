#include "command_line_parser.h"

namespace frontendNS {

cl::OptionCategory Frontend::toolCategory("Qt-obfuscator common options");

cl::opt<string>
    Frontend::qtPath("libraries",
                     cl::desc("path to qt sources folder "
                              "(default=/usr/include/x86_64-linux-gnu/qt5)"),
                     cl::value_desc("folder"), cl::cat(toolCategory),
                     cl::init(Opts::defaultQtPath));
cl::alias Frontend::qtPathAlias("l", cl::desc("alias for --libraries"),
                                cl::aliasopt(qtPath), cl::cat(toolCategory));

cl::list<string> Frontend::qmlFiles("qml",
                                    cl::desc("qml files to be owerwritten"),
                                    cl::value_desc("qml files"),
                                    cl::cat(toolCategory));

cl::opt<bool> Frontend::inplace(
    "inplace", cl::desc("Overwrite edited <file>s (default=0)"),
    cl::value_desc("1 is overwrite, otherwise is create new ones"),
    cl::cat(Frontend::toolCategory), cl::init(0));
cl::alias Frontend::inplaceAlias("i", cl::desc("alias for --inplace"),
                                 cl::aliasopt(inplace), cl::cat(toolCategory));

cl::opt<string> Frontend::exportFixes(
    "export-fixes",
    cl::desc("YAML file to store suggested fixes in (in case of --inplace=1)"),
    cl::value_desc("filename"), cl::cat(toolCategory));
cl::alias Frontend::exportFixesAlias("f", cl::desc("alias for --export-fixes"),
                                     cl::aliasopt(exportFixes),
                                     cl::cat(toolCategory));

cl::opt<Enums::OptEncryprtion> Frontend::encryptionMode(
    "encryption-mode",
    cl::desc("Choose encryption mode for literal encoding (default=RANDOM)"),
    cl::values(clEnumValN(Enums::OptEncryprtion::PJW, "PJW",
                          "PJW-32 encryption"),
               clEnumValN(Enums::OptEncryprtion::MD5, "MD5", "MD5 encryption"),
               clEnumValN(Enums::OptEncryprtion::RANDOM, "RAND",
                          "Random declarations creation")),
    cl::cat(toolCategory));
cl::alias Frontend::encryptionModeAlias("e",
                                        cl::desc("alias for --encryption-mode"),
                                        cl::aliasopt(encryptionMode),
                                        cl::cat(toolCategory));

cl::opt<int> Frontend::RNGSeed(
    "seed", cl::desc("Random Number Generator seed"),
    cl::value_desc("To repeat some results use the same seed"),
    cl::cat(toolCategory), cl::init(0));

llvm::Expected<tooling::CommonOptionsParser>
Frontend::create(int argc, const char **argv, StringRef overwiew) {
  auto expectedParser = llvm::Expected<tooling::CommonOptionsParser>(
      tooling::CommonOptionsParser::create(
          argc, argv, toolCategory, cl::OneOrMore, Opts::generalInfo.c_str()));
  if (!expectedParser)
    return expectedParser.takeError();
  if (auto E = isValidOptions().takeError())
    return std::move(E);
  return expectedParser;
}

Expected<bool> Frontend::isValidOptions() {
  if (RNGSeed < 0)
    return createStringError(inconvertibleErrorCode(),
                             "Invalid rng-seed: it must be >=0.");
  if (!exportFixes.empty()) {
    error_code EC;
    raw_fd_ostream OS{exportFixes, EC, sys::fs::OF_None};
    if (EC)
      return createStringError(inconvertibleErrorCode(),
                               "Error YAML file: " + EC.message());
  }

  if (!qmlFiles.empty()) {
    FileSystemOptions options{"/"};
    FileManager FM{options};
    for (const auto &qmlfile : qmlFiles) {
      auto file = FM.getFileRef(qmlfile, false, false);
      if (auto E = file.takeError()) {
        errs() << "Error QML file: ";
        return std::move(E);
      }
      if (qmlfile.substr(qmlfile.size() - 4, 4) != ".qml")
        return createStringError(inconvertibleErrorCode(),
                                 "Invalid QML file format.");
    }
  }

  return true;
}
} // namespace frontendNS