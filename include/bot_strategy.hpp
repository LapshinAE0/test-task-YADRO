#ifndef BOT_STRATEGY_H
#define BOT_STRATEGY_H

#include "action.hpp"

class Player;
class Logger;

class IStrategyBot {
public:
    virtual ~IStrategyBot() = default;
    virtual Action nextAction(Player& player, Logger& logger) = 0;
};

#endif
