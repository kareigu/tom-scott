#include "commands.hpp"
#include <spdlog/spdlog.h>

namespace ts {
TS_SLASHCOMMAND_IMPL(where) {
  return dpp::slashcommand("where", "Where is Tom?", id())
          .add_option(dpp::command_option(dpp::command_option_type::co_user, "user", "with who?", false));
}

TS_EXECUTE_IMPL(where) {
  try {
    auto user_id = std::get<dpp::snowflake>(event.get_parameter("user"));
    auto user = event.command.get_resolved_user(user_id);
    event.reply(fmt::format("{} is a nigger ({})", user.global_name, user.has_nitro_full() ? "turbo" : "ei turbo"));
  } catch (std::bad_variant_access e) {
    event.reply("Nigger");
  } catch (std::exception e) {
    event.reply("Error");
    return cpp::fail(fmt::format("Error parsing in CommandWhere::execute {}", e.what()));
  }
  return {};
}

}// namespace ts
