#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include "resource.hpp"

class Player;

class Logger {
private:
    std::ofstream result_file;

public:
    Logger() = default;
    ~Logger();

    bool init(const std::string& output_file);
    void logGo(int room);
    void logCollect(const std::string& resource);
    void logState(const Player& player);
    void logResult(const Player& player);
    void logString(const std::string& str);
    void logStringFinish(const std::string& str);
};

#endif
