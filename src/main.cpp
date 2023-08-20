#include "appcommand.h"
#include "config.hpp"

#include <cstdlib>
#include <dpp/dpp.h>
#include <dpp/misc-enum.h>
#include <dpp/once.h>
#include <memory>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <variant>


int main(int argc, char** argv) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif
  auto logger = spdlog::stdout_color_mt("tom-scott");
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("[%Y-%m-%d|%T|%z] [%n] [%^%=7l%$] [thread %t] %v");

  auto config_ret = ts::Config::read_from_file("bot.cfg");
  if (config_ret.has_error()) {
    spdlog::error("Failed reading config file: {}",
                  config_ret.error().to_string());
    return EXIT_FAILURE;
  }
  ts::Config config = config_ret.value();

  auto guild_id_ret = config.get_number("GUILD_ID");
  if (guild_id_ret.has_error()) {
    spdlog::error("Error getting GUILD_ID: {}", guild_id_ret.error().to_string());
    return EXIT_FAILURE;
  }
  const auto guild_id = dpp::snowflake(guild_id_ret.value());

  auto token_ret = config.get_string("TOKEN");
  if (token_ret.has_error()) {
    spdlog::error("Error getting TOKEN: ()", token_ret.error().to_string());
    return EXIT_FAILURE;
  }
  auto token = token_ret.value();
  spdlog::debug(token);
  dpp::cluster bot(token);

  auto dpp_logger = spdlog::stdout_color_mt("dpp");
  bot.on_log([dpp_logger](const dpp::log_t& log) {
    switch (log.severity) {
      case dpp::ll_info:
        dpp_logger->info(log.message);
        break;
      case dpp::ll_warning:
        dpp_logger->warn(log.message);
        break;
      case dpp::ll_error:
        dpp_logger->error(log.message);
        break;
      case dpp::ll_debug:
        dpp_logger->debug(log.message);
        break;
      case dpp::ll_trace:
        dpp_logger->trace(log.message);
        break;
      case dpp::ll_critical:
        dpp_logger->critical(log.message);
        break;
    }
  });

  bot.on_slashcommand([](const dpp::slashcommand_t& event) {
    if (event.command.get_command_name() == "test") {
      event.reply("Nig");
    }

    if (event.command.get_command_name() == "where") {
      auto interaction = std::get<dpp::command_interaction>(event.command.data);

      try {
        auto user_id = std::get<dpp::snowflake>(event.get_parameter("user"));
        auto user = event.command.get_resolved_user(user_id);
        event.reply(fmt::format("{} is a nigger ({})", user.global_name, user.has_nitro_full() ? "turbo" : "ei turbo"));
      } catch (std::bad_variant_access e) {
        event.reply("Nigger");
      } catch (std::exception e) {
        event.reply("Error");
        spdlog::error("Error parsing command {}", e.what());
      }
    }
  });

  bot.on_ready([&bot, guild_id](auto event) {
    if (dpp::run_once<struct register_bot_commands>()) {
      bot.guild_command_create(dpp::slashcommand("test", "n", bot.me.id),
                               guild_id);
      auto where_command = dpp::slashcommand("where", "Where is Tom?", bot.me.id)
                                   .add_option(dpp::command_option(dpp::command_option_type::co_user, "user", "with who?", false));
      bot.guild_command_create(where_command, guild_id);
    }
  });

  bot.start(dpp::st_wait);

  return EXIT_SUCCESS;
}
