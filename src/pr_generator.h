#ifndef PSGENERATOR_H_INCLUDED_
#define PSGENERATOR_H_INCLUDED_

#include "constants.h"
#include "llvm/Support/CommandLine.h"

using namespace std;
using namespace Enums;
using namespace llvm;

class PRGenerator {
public:
  static vector<string>
  generate(const vector<string> &prevNames,
           const OptEncryprtion method = OptEncryprtion::MD5);

  PRGenerator() = delete;
  PRGenerator(const PRGenerator &) = delete;
  PRGenerator(PRGenerator &&) = delete;
  PRGenerator &operator=(const PRGenerator &) = delete;
  PRGenerator &operator=(PRGenerator &&) = delete;
};

#endif /* PSGENERATOR_H_INCLUDED_ */