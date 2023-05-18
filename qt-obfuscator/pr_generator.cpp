#include "pr_generator.h"
#include "llvm/Support/MD5.h"
#include <algorithm>

static void md5_encryption(const std::string &name,
                           std::vector<std::string> &derived_names) {
  std::string new_name{""};
  llvm::MD5 md5{};

  for (const auto &derived : derived_names) {
    if (derived == new_name) {
      char extra = rand();
    }
  }
}

static void pjw_encryption(const std::string &name,
                           std::vector<std::string> &derived_names) {
  std::string new_name{""};
}

static void random_encryption(const std::string &name,
                              std::vector<std::string> &derived_names) {
  std::string new_name{""};
  // Выбираем размер
  size_t decl_size{MIN_SIZE + rand() % (MAX_SIZE - MIN_SIZE + 1)};
  size_t allowed_size{kAllowedSymbols.size()};
  // Добавляем случайный символ. Первым должна быть латинская буква в lowerCase
  /* С учетом неиспользования верхнего регистра и цифр */
  size_t first{rand() % (allowed_size - 37)};
  new_name.push_back((char)kAllowedSymbols[first]);
  for (size_t i = 0; i < decl_size - 1; ++i) {
    size_t symbol = kAllowedSymbols[rand() % allowed_size];
    new_name.push_back(symbol);
  }
  // Дополняем еще случайными символами, если такое имя уже есть
  while (std::find(derived_names.begin(), derived_names.end(), new_name) !=
         derived_names.end()) {
    size_t extra = kAllowedSymbols[rand() % allowed_size];
    new_name.push_back(extra);
  }
  new_name.push_back('_');
  derived_names.push_back(new_name);
}

std::vector<std::string>
PRGenerator::generate(const std::vector<std::string> &prev_names,
                      const OptEncryprtion method) {
  std::vector<std::string> new_names;
  for (const auto &name : prev_names) {
    std::string new_name{""};
    switch (method) {
    case OptEncryprtion::MD5:
      md5_encryption(name, new_names);
      break;
    case OptEncryprtion::PJW:
      pjw_encryption(name, new_names);
      break;
    case OptEncryprtion::RANDOM:
      random_encryption(name, new_names);
    }
  }
  return new_names;
}
