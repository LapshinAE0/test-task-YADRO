#include "logger.hpp"
#include "player.hpp"
#include <algorithm>

Logger::~Logger() {
    if (result_file.is_open()) {
        result_file.flush();
        result_file.close();
    }
}

bool Logger::init(const std::string& output_file) {
    result_file.open(output_file);
    return result_file.is_open();
}

void Logger::logGo(int room) {
    result_file << "go " << room << std::endl;
}

void Logger::logCollect(const std::string& resource) {
    result_file << "collect " << resource << std::endl;
}

void Logger::logState(const Player& player) {
    const int number_room = player.current_room;
    if (number_room == 0) return;

    std::string resource;
    std::vector<Resource> collected_res = player.visited_rooms.at(number_room).getResourcesThatCollected();

    bool find_iron = std::find(collected_res.begin(), collected_res.end(), Resource::IRON) != collected_res.end();
    bool find_gold = std::find(collected_res.begin(), collected_res.end(), Resource::GOLD) != collected_res.end();
    bool find_gems = std::find(collected_res.begin(), collected_res.end(), Resource::GEMS) != collected_res.end();
    bool find_exp  = std::find(collected_res.begin(), collected_res.end(), Resource::EXP)  != collected_res.end();

    resource += find_iron ? "_" : std::to_string(player.visited_rooms.at(number_room).getRoom().iron);
    resource += " ";
    resource += find_gold ? "_" : std::to_string(player.visited_rooms.at(number_room).getRoom().gold);
    resource += " ";
    resource += find_gems ? "_" : std::to_string(player.visited_rooms.at(number_room).getRoom().gems);
    resource += " ";
    resource += find_exp  ? "_" : std::to_string(player.visited_rooms.at(number_room).getRoom().exp);

    result_file << "state " << player.current_room << " " << resource << std::endl;
}

void Logger::logResult(const Player& player) {
    result_file << "result " 
                << player.collected_iron << " " 
                << player.collected_gold << " " 
                << player.collected_gems << " " 
                << player.collected_exp << " " 
                << player.totalvalue();
}

void Logger::logString(const std::string& str) {
    result_file << str << std::endl;
}

void Logger::logStringFinish(const std::string& str) {
    result_file << str;
}