#pragma once
#include "utils.hpp"
#include <filesystem>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <result.hpp>
#include <spdlog/fmt/fmt.h>
#include <string>
#include <variant>

namespace ts {
class Config {
public:
  using ConfigValue = std::variant<uint64_t, std::string>;
  using StorageType = std::map<std::string, ConfigValue>;
  using Error = utils::Error;


  static cpp::result<Config, Error>
  read_from_file(const char* filepath) noexcept;

  cpp::result<std::string, Error> get_string(const std::string& key) const noexcept;
  cpp::result<uint64_t, Error> get_number(const std::string& key) const noexcept;


  ~Config() = default;

  std::vector<const char*> guaranteed_keys = {"TOKEN"};

private:
  Config() = default;

  cpp::result<void, Error> read_from_path(const std::filesystem::path& path) noexcept;

  StorageType m_data;
};
}// namespace ts
