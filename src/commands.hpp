#pragma once
#include <dpp/dpp.h>
#include <dpp/snowflake.h>
#include <result.hpp>
#include <spdlog/fmt/fmt.h>

namespace ts {
#define TS_SLASHCOMMAND_FUNCTION virtual dpp::slashcommand to_slashcommand() const noexcept
#define TS_SLASHCOMMAND_IMPL(NAME) dpp::slashcommand Command##NAME ::to_slashcommand() const noexcept
#define TS_EXECUTE_FUNCTION virtual cpp::result<void, std::string> execute(dpp::slashcommand_t event) const noexcept
#define TS_EXECUTE_IMPL(NAME) cpp::result<void, std::string> Command##NAME ::execute(dpp::slashcommand_t event) const noexcept
#define TS_CREATE_COMMAND_CLASS(NAME)                                         \
  class Command##NAME : public Command {                                      \
  public:                                                                     \
    Command##NAME(dpp::snowflake id) : Command(id) {}                         \
    ~Command##NAME() = default;                                               \
    TS_SLASHCOMMAND_FUNCTION final;                                           \
    TS_EXECUTE_FUNCTION final;                                                \
    virtual const std::string& name() const noexcept final { return m_name; } \
                                                                              \
  private:                                                                    \
    std::string m_name = #NAME;                                               \
  };// namespace ts
class Command {
public:
  TS_SLASHCOMMAND_FUNCTION = 0;
  TS_EXECUTE_FUNCTION = 0;

  virtual const std::string& name() const noexcept = 0;

  const dpp::snowflake& id() const { return m_id; }

  Command(dpp::snowflake id) : m_id(id) {}
  ~Command() = default;

private:
  dpp::snowflake m_id;
};

struct Response {
  std::string location;
  std::optional<std::string> with;
  std::optional<std::string> looking_at;
  static cpp::result<Response, std::string> generate_response(const dpp::slashcommand_t& event);
};

TS_CREATE_COMMAND_CLASS(where)

}// namespace ts
template<>
struct fmt::formatter<ts::Response> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const ts::Response& response, FormatContext& ctx) {
    fmt::format_to(ctx.out(), "I am {}", response.location);
    if (response.with) {
      fmt::format_to(ctx.out(), " {}", response.with.value());
    }
    if (response.looking_at) {
      fmt::format_to(ctx.out(), " {}", response.looking_at.value());
    }
    return fmt::format_to(ctx.out(), ".");
  }
};
