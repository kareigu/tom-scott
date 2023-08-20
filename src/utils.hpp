#pragma once
#include <filesystem>
#include <optional>
#include <result.hpp>
#include <string>

namespace ts {
namespace utils {
  class Error {
  public:
    enum class Type {
      NoFile,
      SyntaxError,
      IOError,
      ValueError,
      ParseError,
      UnknownError,
    };
    using Type::IOError;
    using Type::NoFile;
    using Type::ParseError;
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
  cpp::result<std::filesystem::path, Error> validate_path(const char* path) noexcept;
}// namespace utils
}// namespace ts
