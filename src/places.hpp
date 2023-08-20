#pragma once
#include "utils.hpp"
#include <result.hpp>

namespace ts {
struct Place {
  std::string first_name;
  std::string eu_name;
  std::string last_name;
  std::string buzzword;
  std::string gender;
  std::string race;
  std::string product;
  std::string material;
  std::string animal;
  std::string airport_name;
  std::string street_name;
  std::string city;
  std::string country;
  std::string domain;
  std::string drug_company;
  std::string company;
  std::string fake_company;
  std::string stock;

  bool is_filled() const;
};

class Places {
public:
  using Error = utils::Error;


  static cpp::result<void, Error> init(const char* data_path);

  static const std::vector<Place>& data() { return m_data; }

  Places() = delete;
  ~Places() = delete;

private:
  inline static std::vector<Place> m_data;
};
}// namespace ts
