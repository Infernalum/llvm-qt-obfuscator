#ifndef PSGENERATOR_H_INCLUDED_
#define PSGENERATOR_H_INCLUDED_

#include "constants.h"

using namespace Enums;
using namespace RNGOpts;

class PRGenerator {
public:
  static std::vector<std::string>
  generate(const std::vector<std::string> &prev_names,
           const OptEncryprtion method = OptEncryprtion::MD5);

  PRGenerator() = delete;
  PRGenerator(const PRGenerator &) = delete;
  PRGenerator(PRGenerator &&) = delete;
  PRGenerator &operator=(const PRGenerator &) = delete;
  PRGenerator &operator=(PRGenerator &&) = delete;
};

#endif /* PSGENERATOR_H_INCLUDED_ */