#pragma once
#include <filesystem>
#include <map>
#include <optional>
#include <result.hpp>
#include <spdlog/fmt/fmt.h>
#include <string>

namespace ts {
class Config {
public:
  union ConfigValue {
    uint64_t number;
    const char* string;
  };

  class Error {
  public:
    enum class Type {
      NoFile,
      SyntaxError,
      UnknownError,
    };
    using Type::NoFile;
    using Type::SyntaxError;
    using Type::UnknownError;

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

  ~Config() = default;

private:
  Config() = default;

  static cpp::result<std::filesystem::path, Error> validate_path(const char* path) noexcept;

  std::map<std::string, ConfigValue> m_data;
};
}// namespace ts
