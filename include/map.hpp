#ifndef MAP_H
#define MAP_H

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include "room.hpp"
#include "resource.hpp"

class Logger;

class Map {
public:
    Map() = default;
    ~Map() = default;

    bool readDataFromFile(std::string file_name, Logger& logger);
    Room* getRoom(int id);
    int getFood() const;
    Resource getTargetResource() const;

private:
    int N;
    std::map<int, Room> rooms;
    int food = 0;
    Resource target_resource = Resource::IRON;

    bool parseResource(const std::string& str, Resource& res);
    std::string trim(const std::string& s);
    bool checkStrIsPosNumber(const std::string& str);
    bool getNumberFromStr(const std::string& line, int& val, bool check_limits = false,
                          int min_lim = 0, int max_lim = 255);
    bool parsStrByCharacters(const std::string& str, std::vector<int>& result_list, char character);
    bool getAdjoiningRoomsFromStr(const std::string& str, std::vector<int>& neighbors, int maxN);
    std::vector<std::string> splitBySpaces(const std::string& str);
    bool parseCountRooms(std::ifstream& file_input, Logger& logger);
    bool parseInfoOfRooms(std::ifstream& file_input, Logger& logger);
    bool parseFoodAndTargetResource(std::ifstream& file_input, Logger& logger);
};

#endif
