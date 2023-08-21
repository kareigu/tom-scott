#include "commands.hpp"
#include "places.hpp"
#include <chrono>
#include <exception>
#include <optional>
#include <random>
#include <spdlog/spdlog.h>
#include <string>
#include <variant>

namespace ts {
TS_SLASHCOMMAND_IMPL(where) {
  return dpp::slashcommand("where", "Where is Tom?", id())
          .add_option(dpp::command_option(dpp::command_option_type::co_user, "user", "with who?", false));
}


cpp::result<Response, std::string> Response::generate_response(const dpp::slashcommand_t& event) {
  std::mt19937_64 rand(std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<size_t> place_generator(0, Places::data().size() - 1);
#define TS_GET_VALUE(member)                                         \
  std::string member = Places::data()[place_generator(rand)].member; \
  {                                                                  \
    uint8_t retries = 0;                                             \
    if (member.length() <= 0) {                                      \
      while (retries < 5) {                                          \
        member = Places::data()[place_generator(rand)].member;       \
        if (member.length() > 0)                                     \
          break;                                                     \
      }                                                              \
      if (retries >= 5) {                                            \
        event.reply("I'm in pain");                                  \
        return cpp::fail("Failed to get a valid roll with 5 tries"); \
      }                                                              \
    }                                                                \
  }
  TS_GET_VALUE(street_name);
  TS_GET_VALUE(city);
  TS_GET_VALUE(country);
  TS_GET_VALUE(airport_name)
  Response response;
  std::string& location_string = response.location;
  std::uniform_int_distribution<uint8_t> location_generator(0, 12);
  switch (location_generator(rand)) {
    case 0:
    case 1:
    case 2:
    case 3:
      location_string = fmt::format("in {}, {}", city, country);
      break;
    case 4:
    case 5:
      location_string = fmt::format("on {} in {}, {}", street_name, city, country);
      break;
    case 6:
    case 7:
      location_string = fmt::format("on {} in {}", street_name, country);
      break;
    case 10:
    case 12:
      location_string = fmt::format("on {} in {}", street_name, city);
      break;
    case 8:
      location_string = fmt::format("in {}", city);
      break;
    case 9:
      location_string = fmt::format("at {} in {}", airport_name, city);
      break;
    case 11:
      location_string = fmt::format("at {} in {}", airport_name, country);
      break;
  }

  std::optional<std::string> name_string;
  try {
    auto user_id = std::get<dpp::snowflake>(event.get_parameter("user"));
    name_string = event.command.get_resolved_user(user_id).get_mention();
  } catch (std::bad_variant_access e) {
    std::uniform_int_distribution<uint8_t> include_name_generator(0, 10);
    if (include_name_generator(rand) > 6) {
      std::string full_name;
      std::uniform_int_distribution<uint8_t> name_generator(0, 2);
      TS_GET_VALUE(first_name);
      TS_GET_VALUE(eu_name);
      switch (name_generator(rand)) {
        case 0:
        case 1:
          full_name.append(first_name);
          break;
        case 2:
          full_name.append(eu_name);
          break;
      }
      TS_GET_VALUE(last_name);
      switch (name_generator(rand)) {
        case 0:
        case 1:
          full_name.append(" ");
          full_name.append(last_name);
          break;
        case 2:
          break;
      }
      name_string = full_name;
    } else {
      name_string = {};
    }
  } catch (std::exception e) {
    return cpp::fail(e.what());
  }

  if (name_string) {
    std::string name = name_string.value();
    std::optional<std::string>& with = response.with;
    std::uniform_int_distribution<uint8_t> name_generator(0, 18);
    TS_GET_VALUE(company);
    TS_GET_VALUE(race);
    TS_GET_VALUE(drug_company);
    TS_GET_VALUE(stock);
    TS_GET_VALUE(fake_company);
    TS_GET_VALUE(domain);
    switch (name_generator(rand)) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 11:
      case 18:
        with = fmt::format("with {}", name);
        break;
      case 4:
      case 5:
      case 6:
        with = fmt::format("with {} {}", race, name);
        break;
      case 7:
      case 8:
        with = fmt::format("with {} of {}", name, stock);
        break;
      case 9:
      case 10:
        with = fmt::format("with {} from {}", name, company);
        break;
      case 12:
      case 13:
        with = fmt::format("with {} from {}", name, company);
        break;
      case 14:
      case 15:
        with = fmt::format("with {} of {}", name, fake_company);
        break;
      case 16:
      case 17:
        with = fmt::format("with {} from {}", name, domain);
        break;
    }
  }

  return response;
}

TS_EXECUTE_IMPL(where) {
  auto response_ret = Response::generate_response(event);
  if (response_ret.has_error()) {
    event.reply("I'm in pain");
    return cpp::fail(response_ret.error());
  }
  Response response = response_ret.value();
  event.reply(fmt::format("{}", response));
  return {};
}

}// namespace ts
