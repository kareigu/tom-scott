#pragma once
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

  class Error {
  public:
    enum class Type {
      NoFile,
      SyntaxError,
      IOError,
      ValueError,
      UnknownError,
    };
    using Type::IOError;
    using Type::NoFile;
    using Type::SyntaxError;
    using Type::UnknownError;
    using Type::ValueError;

    Error(Type type, std::string message = "");
    ~Error() = default;

    const std::string to_string() const;

    Error() : m_type(Type::UnknownError){};

  private:
    std::optional<std::string> m_message;
    Type m_type;
  };

  static cpp::result<Config, Error>
  read_from_file(const char* filepath) noexcept;

  cpp::result<std::string, Error> get_string(const std::string& key) const noexcept;
  cpp::result<uint64_t, Error> get_number(const std::string& key) const noexcept;


  ~Config() = default;

  std::vector<const char*> guaranteed_keys = {"TOKEN"};

private:
  Config() = default;

  static cpp::result<std::filesystem::path, Error> validate_path(const char* path) noexcept;
  cpp::result<void, Error> read_from_path(const std::filesystem::path& path) noexcept;

  StorageType m_data;
};
}// namespace ts
