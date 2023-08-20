#include "commands.hpp"
#include "places.hpp"
#include <chrono>
#include <random>
#include <spdlog/spdlog.h>

namespace ts {
TS_SLASHCOMMAND_IMPL(where) {
  return dpp::slashcommand("where", "Where is Tom?", id())
          .add_option(dpp::command_option(dpp::command_option_type::co_user, "user", "with who?", false));
}

TS_EXECUTE_IMPL(where) {
  std::mt19937_64 rand(std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<size_t> generator(0, Places::data().size() - 1);


  Place roll = Places::data()[generator(rand)];
  uint8_t retries = 0;
  if (!roll.is_filled()) {
    while (retries < 5) {
      roll = Places::data()[generator(rand)];
      if (roll.is_filled())
        break;
    }
    if (retries >= 5) {
      event.reply("I'm in pain");
      return cpp::fail("Failed to get a valid roll with 5 tries");
    }
  }

  std::uniform_int_distribution<uint8_t> adjective_generator(0, 10);
  std::string adjective;
  switch (adjective_generator(rand)) {
    case 0:
    case 1:
    case 2:
      adjective = roll.race;
      break;
    case 3:
      adjective = roll.material;
      break;
    case 4:
    case 5:
    case 6:
      adjective = roll.gender;
      break;
    case 7:
      adjective = roll.animal;
      break;
    case 8:
    case 9:
    case 10:
      adjective = roll.buzzword;
      break;
  }

  try {
    auto user_id = std::get<dpp::snowflake>(event.get_parameter("user"));
    auto user = event.command.get_resolved_user(user_id);
    event.reply(fmt::format("I'm in {} with {}{}", roll.city, generator(rand) % 2 == 0 ? "" : fmt::format("{} ", adjective), user.global_name));
  } catch (std::bad_variant_access e) {
    event.reply(fmt::format("I'm in {}, {}", roll.city, roll.country));
  } catch (std::exception e) {
    event.reply("Error");
    return cpp::fail(fmt::format("Error parsing in CommandWhere::execute {}", e.what()));
  }
  return {};
}

}// namespace ts
