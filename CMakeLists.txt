cmake_minimum_required(VERSION 3.20)

include(FetchContent)

project(
  tom-scott
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_GENERATOR STREQUAL "Ninja Multi-Config")
  set(NINJA_MULTICONFIG TRUE)
else()
  set(NINJA_MULTICONFIG FALSE)
endif()

set(SOURCES
    src/main.cpp
    src/config.cpp
    src/commands.cpp
    src/places.cpp
    src/utils.cpp
)

fetchcontent_declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.12.0
  GIT_PROGRESS TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
fetchcontent_makeavailable(spdlog)

fetchcontent_declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
  GIT_PROGRESS TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
fetchcontent_makeavailable(nlohmann_json)

set(BUILD_VOICE_SUPPORT OFF)
set(DPP_INSTALL OFF)
set(DPP_BUILD_TEST OFF)
set(DPP_NO_VCPKG ON)
fetchcontent_declare(
  dpp
  GIT_REPOSITORY https://github.com/kareigu/DPP.git
  GIT_TAG master
  GIT_PROGRESS TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
fetchcontent_makeavailable(dpp)

fetchcontent_declare(
  Result
  GIT_REPOSITORY https://github.com/bitwizeshift/result.git
  GIT_TAG master
  GIT_PROGRESS TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)
fetchcontent_makeavailable(Result)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE spdlog
          nlohmann_json
          dpp
          Result
)

if(NINJA_MULTICONFIG)
  set(OUTPUT_FILE_PATH "./Debug/${PROJECT_NAME}")
else()
  set(OUTPUT_FILE_PATH "./${PROJECT_NAME}")
endif()

add_custom_target(
  run
  COMMAND ${CMAKE_COMMAND} -E env ${OUTPUT_FILE_PATH}
  USES_TERMINAL
  DEPENDS ${PROJECT_NAME}
)

if(NINJA_MULTICONFIG)
  add_custom_target(
    release
    COMMAND ${CMAKE_COMMAND} -E env "ninja" "-f" "build-Release.ninja"
    USES_TERMINAL
  )
endif()

if(DOCKER_BUILD)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:dpp>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )
endif()
