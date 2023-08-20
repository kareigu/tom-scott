#include "places.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <sstream>

namespace ts {
using Error = Places::Error;
cpp::result<void, Error> Places::init(const char* _data_path) {
  m_data.resize(1000);

  auto data_path_ret = utils::validate_path(_data_path);
  if (data_path_ret.has_error())
    return cpp::fail(data_path_ret.error());

  auto data_path = data_path_ret.value();
  std::ifstream data_file(data_path);
  if (!data_file.is_open())
    return cpp::fail(Error(Error::IOError, "Couldn't open file for reading"));
  try {
    std::vector<nlohmann::json> json_data = nlohmann::json::parse(data_file);

    for (const auto& j : json_data) {
      m_data.emplace_back(Place{
              .first_name = j["first_name"],
              .eu_name = j["eu_name"],
              .last_name = j["last_name"],
              .buzzword = j["buzzword"],
              .gender = j["gender"],
              .race = j["race"],
              .product = j["product"],
              .material = j["material"],
              .animal = j["animal"],
              .airport_name = j["airport_name"],
              .street_name = j["street_name"],
              .city = j["city"],
              .country = j["country"],
              .domain = j["domain"],
              .drug_company = j["drug_company"],
              .company = j["company"],
              .fake_company = j["fake_company"],
              .stock = j["stock"],
      });
    }
    return {};

  } catch (nlohmann::json::parse_error e) {
    return cpp::fail(Error(Error::ParseError, fmt::format("Couldn't parse json: {} - character: {}", e.what(), e.byte)));
  } catch (nlohmann::json::exception e) {
    return cpp::fail(Error(Error::UnknownError, fmt::format("Unknown error while reading json file: {}", e.what())));
  }
}

bool Place::is_filled() const {
  auto f = [](const std::string& member) { return member.size() > 0; };
  return f(first_name) && f(eu_name) && f(last_name) && f(buzzword) && f(gender) && f(race) && f(product) && f(material) && f(animal) && f(airport_name) && f(street_name) && f(city) && f(country) && f(domain) && f(drug_company) && f(company) && f(fake_company) && f(stock);
}
}// namespace ts
