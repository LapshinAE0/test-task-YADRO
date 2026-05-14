cmake_minimum_required(VERSION 3.10)
project(TerracraftValrising VERSION 1.0 LANGUAGES CXX)

add_executable(game
    src/main.cpp
    src/player.cpp
    src/map.cpp
    src/logger.cpp
    src/alice_bot.cpp
    src/game.cpp
)

target_include_directories(game PRIVATE include)
