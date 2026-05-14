#ifndef GAME_H
#define GAME_H

#include <string>
#include "map.hpp"
#include "player.hpp"
#include "logger.hpp"
#include "bot_strategy.hpp"

enum class StageGame { WIN, LOSS, CONTINUING };

class Game {
public:
    Logger logger;
    Map map;
    Player player;
    IStrategyBot* bot = nullptr;

    Game() = default;
    ~Game() = default;

    void startGameBot();
    void setBot(IStrategyBot* strategy);
    bool goToRoom(int room_id);
    bool init(const std::string& input_filename, const std::string& output_file = "result.txt");
    bool collectBestResource();
    bool collectResource(Resource res);
    StageGame gameStatus();
    bool playerIsWin();
    bool playerIsDeath();
};

#endif
