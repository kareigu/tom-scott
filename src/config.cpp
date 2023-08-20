#include "config.hpp"
#include <fstream>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <variant>

namespace ts {
using Error = Config::Error;
cpp::result<Config, Error>
Config::read_from_file(const char* filepath) noexcept {
  Config config;

  auto config_path_ret = utils::validate_path(filepath);
  if (config_path_ret.has_error())
    return cpp::fail(config_path_ret.error());
  auto config_path = config_path_ret.value();

  spdlog::info("Found config file at {}", config_path.string());

  auto ret = config.read_from_path(config_path);
  if (ret.has_error())
    return cpp::fail(ret.error());

  return config;
}


cpp::result<void, Error> Config::read_from_path(const std::filesystem::path& path) noexcept {
  std::ifstream input_stream(path);
  if (!input_stream.is_open())
    return cpp::fail(Error(Error::IOError, "Couldn't open config file for reading"));


  std::string line;
  for (size_t line_nr = 1; std::getline(input_stream, line); line_nr++) {
    if (line.length() == 0)
      continue;

    size_t separator_pos = line.find_first_of('=');
    if (separator_pos == std::string::npos)
      return cpp::fail(Error(Error::SyntaxError, fmt::format("Missing '=' in key declaration:line {}", line_nr)));

    std::string key;
    key.reserve(separator_pos);
    for (size_t i = 0; i < separator_pos; i++) {
      char c = line[i];
      if (c >= 'a' && c <= 'z')
        return cpp::fail(Error(Error::SyntaxError, fmt::format("Invalid key name, should be capitalized:line {}", line_nr)));
      key.push_back(line[i]);
    }

    std::string value;
    value.reserve(line.length() - separator_pos);
    bool forced_string = false;
    for (size_t i = separator_pos + 1; i < line.length(); i++) {
      char c = line[i];
      if (c == '"' || c == '\'') {
        forced_string = true;
        continue;
      }

      value.push_back(c);
    }
    ConfigValue config_value;
    if (forced_string) {
      config_value = std::move(value);
    } else {

      try {
        uint64_t num_value = std::stoll(value);
        config_value = num_value;
      } catch (std::invalid_argument e) {
        config_value = std::move(value);
      } catch (std::out_of_range e) {
        return cpp::fail(
                Error(
                        Error::SyntaxError,
                        fmt::format("Config value too long. Turn it into a string or provide a smaller number:line {}", line_nr)));
      }
    }

    try {
      m_data[key] = std::move(config_value);
    } catch (std::exception e) {
      return cpp::fail(Error(Error::UnknownError, e.what()));
    }
  }

  for (const auto& key : guaranteed_keys) {
    if (!m_data.contains(key))
      return cpp::fail(Error(Error::SyntaxError, fmt::format("Missing key: {} in config", key)));
  }

  return {};
}


static inline cpp::result<Config::ConfigValue, Error> get_config_value(const std::string& key, const Config::StorageType& data) noexcept {
  Config::ConfigValue value;
  try {
    value = data.at(key);
  } catch (std::exception e) {
    return cpp::fail(Error(Error::ValueError, fmt::format("No value with key {}", key)));
  }
  return value;
}

cpp::result<std::string, Error> Config::get_string(const std::string& key) const noexcept {
  auto value_ret = get_config_value(key, m_data);
  if (value_ret.has_error())
    return cpp::fail(value_ret.error());
  auto value = value_ret.value();

  try {
    auto string = std::get<std::string>(value);
    return string;
  } catch (std::bad_variant_access e) {
    return cpp::fail(Error(Error::ValueError, "Value not a string"));
  }
}

cpp::result<uint64_t, Error> Config::get_number(const std::string& key) const noexcept {
  auto value_ret = get_config_value(key, m_data);
  if (value_ret.has_error())
    return cpp::fail(value_ret.error());
  auto value = value_ret.value();

  try {
    uint64_t number = std::get<uint64_t>(value);
    return number;
  } catch (std::bad_variant_access e) {
    return cpp::fail(Error(Error::ValueError, "Value not a string"));
  }
}

Error::Error(Error::Type type, std::string message) : m_type(type) {
  if (message.length() > 0)
    m_message = std::move(message);
  else
    m_message = {};
}

}// namespace ts
