#include "utils.hpp"
#include <spdlog/spdlog.h>

namespace ts {
namespace utils {
  cpp::result<std::filesystem::path, Error> validate_path(const char* filepath) noexcept {
    auto cwd = std::filesystem::current_path();
    spdlog::debug("cwd: {}", cwd.string());

    auto file = cwd;
    file.append(filepath);

    if (!std::filesystem::exists(file)) {
      file = cwd;
      file = file.parent_path().append(filepath);
      if (!std::filesystem::exists(file))
        return cpp::fail(Error(Error::NoFile, fmt::format("Couldn't find {}", filepath)));
    }

    return file;
  }

  const std::string Error::to_string() const {
    std::string string;
    switch (m_type) {
      case Error::NoFile:
        string.append("NoFile");
        break;
      case Error::SyntaxError:
        string.append("SyntaxError");
        break;
      case Error::IOError:
        string.append("IOError");
        break;
      case Error::ValueError:
        string.append("ValueError");
        break;
      case Error::ParseError:
        string.append("ParseError");
        break;
      case Error::UnknownError:
        string.append("UnknownError");
        break;
    }
    if (!m_message)
      return string;

    return fmt::format("{} - {}", string, m_message.value());
  }
}// namespace utils
}// namespace ts
