#include "config.hpp"
#include <spdlog/spdlog.h>

namespace ts {
using Error = Config::Error;
cpp::result<Config, Error>
Config::read_from_file(const char* filepath) noexcept {
  Config config;

  auto config_path_ret = validate_path(filepath);
  if (config_path_ret.has_error())
    return cpp::fail(config_path_ret.error());
  auto config_path = config_path_ret.value();

  spdlog::debug("Found config file at {}", config_path.string());

  return config;
}

cpp::result<std::filesystem::path, Error> Config::validate_path(const char* filepath) noexcept {
  auto cwd = std::filesystem::current_path();
  spdlog::debug("cwd: {}", cwd.string());

  auto config_path = cwd;
  config_path.append(filepath);

  if (!std::filesystem::exists(config_path)) {
    config_path = cwd;
    config_path = config_path.parent_path().append(filepath);
    if (!std::filesystem::exists(config_path))
      return cpp::fail(Error(Error::NoFile, fmt::format("Couldn't find {}", filepath)));
  }

  return config_path;
}

Error::Error(Error::Type type, std::string message) : m_type(type) {
  if (message.length() > 0)
    m_message = std::move(message);
  else
    m_message = {};
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
    case Error::UnknownError:
      string.append("UnknownError");
      break;
  }
  if (!m_message)
    return string;

  return fmt::format("{} - {}", string, m_message.value());
}
}// namespace ts
