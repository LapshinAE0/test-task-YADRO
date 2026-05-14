#include "map.hpp"
#include "logger.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>


bool Map::readDataFromFile(std::string file_name, Logger& logger) {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Cannot open file" << std::endl;
        return false;
    }

    if (!parseCountRooms(file, logger))      { file.close(); return false; }
    if (!parseInfoOfRooms(file, logger))     { file.close(); return false; }
    if (!parseFoodAndTargetResource(file, logger)) { file.close(); return false; }

    file.close();
    return true;
}

Room* Map::getRoom(int id) {
    auto it = rooms.find(id);
    if (it == rooms.end()) {
        std::cerr << "ERROR: room with this id doesn't exist" << std::endl;
        return nullptr;
    }
    return &it->second;
}

int Map::getFood() const { return food; }

Resource Map::getTargetResource() const { return target_resource; }

bool Map::parseResource(const std::string& str, Resource& res) {
    if (str == "iron") { res = Resource::IRON; return true; }
    if (str == "gold") { res = Resource::GOLD; return true; }
    if (str == "gems") { res = Resource::GEMS; return true; }
    if (str == "exp")  { res = Resource::EXP;  return true; }
    res = Resource::NONE;
    return false;
}

std::string Map::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool Map::checkStrIsPosNumber(const std::string& str) {
    if (str.empty()) return false;
    for (unsigned char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool Map::getNumberFromStr(const std::string& line, int& val, bool check_limits,
                           int min_lim, int max_lim) {
    std::string new_line = trim(line);
    if (!checkStrIsPosNumber(new_line)) return false;

    val = stoi(new_line);

    if (check_limits && (val < min_lim || val > max_lim)) return false;
    return true;
}

bool Map::parsStrByCharacters(const std::string& str, std::vector<int>& result_list, char character) {
    std::stringstream stream(str);
    std::string item;
    while (getline(stream, item, character)) {
        if (!checkStrIsPosNumber(item)) return false;
        result_list.push_back(stoi(item));
    }
    return true;
}

bool Map::getAdjoiningRoomsFromStr(const std::string& str, std::vector<int>& neighbors, int maxN) {
    if (str.empty()) return true;
    if (!parsStrByCharacters(str, neighbors, ',')) return false;

    for (int i : neighbors) {
        if (i < 0 || i > maxN) return false;
    }
    return true;
}

std::vector<std::string> Map::splitBySpaces(const std::string& str) {
    std::vector<std::string> tokens;
    std::stringstream stream(str);
    std::string token;
    while (getline(stream, token, ' ')) {
        tokens.push_back(token);
    }
    return tokens;
}

bool Map::parseCountRooms(std::ifstream& file_input, Logger& logger) {
    std::string line;
    if (!getline(file_input, line)) {
        logger.logStringFinish("");
        return false;
    }
    if (!getNumberFromStr(line, N, true, 1, 255)) {
        logger.logStringFinish(line);
        return false;
    }
    return true;
}

bool Map::parseInfoOfRooms(std::ifstream& file_input, Logger& logger) {
    std::string line;
    for (size_t i = 0; i <= N; i++) {
        if (!getline(file_input, line)) {
            logger.logStringFinish("Missing room data");
            return false;
        }

        std::vector<std::string> parts = splitBySpaces(line);
        if (parts.size() != 6) {
            logger.logStringFinish(line);
            return false;
        }

        Room new_room;
        bool ok = true;
        ok = ok && getNumberFromStr(parts[0], new_room.id, true, 0, N);
        ok = ok && getAdjoiningRoomsFromStr(parts[1], new_room.adjoining_rooms_id, N);
        ok = ok && getNumberFromStr(parts[2], new_room.iron, true, 0, 255);
        ok = ok && getNumberFromStr(parts[3], new_room.gold, true, 0, 255);
        ok = ok && getNumberFromStr(parts[4], new_room.gems, true, 0, 255);
        ok = ok && getNumberFromStr(parts[5], new_room.exp,  true, 0, 255);

        if (!ok) {
            logger.logStringFinish(line);
            return false;
        }

        rooms[new_room.id] = new_room;
    }
    return true;
}

bool Map::parseFoodAndTargetResource(std::ifstream& file_input, Logger& logger) {
    std::string line;
    if (!getline(file_input, line)) {
        logger.logStringFinish("Missing last string with food");
        return false;
    }

    std::vector<std::string> parts = splitBySpaces(line);
    if (parts.size() != 2) {
        logger.logStringFinish(line);
        return false;
    }

    bool food_ok = getNumberFromStr(parts[0], food, true, 2, 255);
    bool res_ok = parseResource(parts[1], target_resource);

    if (!food_ok || !res_ok) {
        logger.logStringFinish(line);
        return false;
    }

    return true;
}
