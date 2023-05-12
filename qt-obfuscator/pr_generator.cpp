#include "pr_generator.h"
#include "llvm/Support/MD5.h"
#include <algorithm>

static void md5_encryption(const std::string &name,
                           std::vector<std::string> &derivedNames) {
  std::string newName{};
  llvm::MD5 md5{};

  for (const auto &derived : derivedNames) {
    if (derived == newName) {
      char extra = rand();
    }
  }
}

static void pjw_encryption(const std::string &name,
                           std::vector<std::string> &derivedNames) {
  std::string newName{};
}

static void random_encryption(const std::string &name,
                              std::vector<std::string> &derivedNames) {
  std::string newName{""};
  // Выбираем размер
  size_t declSize{MIN_SIZE + rand() % (MAX_SIZE - MIN_SIZE + 1)};
  size_t allowedSize{allowedSymbols.size()};
  // Добавляем случайный символ. Первым должна быть латинская буква в lowerCase
  /* С учетом неиспользования верхнего регистра и цифр */
  size_t first{rand() % (allowedSize - 37)};
  newName.push_back((char)allowedSymbols[first]);
  for (size_t i = 0; i < declSize - 1; ++i) {
    size_t symbol = allowedSymbols[rand() % allowedSize];
    newName.push_back(symbol);
  }
  // Дополняем еще случайными символами, если такое имя уже есть
  while (std::find(derivedNames.begin(), derivedNames.end(), newName) !=
         derivedNames.end()) {
    size_t extra = allowedSymbols[rand() % allowedSize];
    newName.push_back(extra);
  }
  newName.push_back('_');
  derivedNames.push_back(newName);
}

std::vector<std::string>
PRGenerator::generate(const std::vector<std::string> &prevNames,
                      const OptEncryprtion method) {
  std::vector<std::string> newNames;
  for (const auto &name : prevNames) {
    std::string newName{};
    switch (method) {
    case OptEncryprtion::MD5:
      md5_encryption(name, newNames);
      break;
    case OptEncryprtion::PJW:
      pjw_encryption(name, newNames);
      break;
    case OptEncryprtion::RANDOM:
      random_encryption(name, newNames);
    }
  }
  return std::move(newNames);
}
