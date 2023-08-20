#include "config.hpp"

#include <cstdlib>
#include <dpp/dpp.h>
#include <dpp/once.h>
#include <memory>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


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

  const auto guild_id = dpp::snowflake(0000);
  bool start_bot = false;
  if (!start_bot)
    return EXIT_SUCCESS;

  dpp::cluster bot("");

  bot.on_slashcommand([](auto event) {
    if (event.command.get_command_name() == "test") {
      event.reply("Nig");
    }
  });

  bot.on_ready([&bot, guild_id](auto event) {
    if (dpp::run_once<struct register_bot_commands>()) {
      bot.guild_command_create(dpp::slashcommand("test", "n", bot.me.id),
                               guild_id);
    }
  });

  bot.start(dpp::st_wait);

  return EXIT_SUCCESS;
}
