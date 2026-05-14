#ifndef ALICE_BOT_H
#define ALICE_BOT_H

#include <vector>
#include "bot_strategy.hpp"
#include "action.hpp"

enum class PhaseBot { RESEARCH, RETURN };

class Player;
class Logger;

class AliceBot : public IStrategyBot {
private:
    int initially_count_food;
    PhaseBot phase;
    std::vector<int> path_to_target;
    size_t path_index = 0;

public:
    AliceBot(int food);
    ~AliceBot() = default;

    Action nextAction(Player& player, Logger& logger) override;

private:
    Action researchAction(Player& player);
    Action returnAction(Player& player);
};

#endif
