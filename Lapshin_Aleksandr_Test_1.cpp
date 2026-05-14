#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>


using namespace std;


const int VALUE_IRON = 7;
const int VALUE_GOLD = 11;
const int VALUE_GEMS = 23;
const int VALUE_EXP = 1;
const int VALUE_NONE = 0;


enum class Resource { IRON, GOLD, GEMS, EXP, NONE};
enum class Visibility { UNKNOWN, SEEN, VISITED };
enum class TypeAction { GO, COLLECT, FINISH };
enum class StageGame { WIN, LOSS, CONTINUING };
enum class PhaseBot { RESEARCH, RETURN };


struct Room {
    int id;
    vector<int> adjoining_rooms_id;
    int iron, gold, gems, exp;

    Room() {}
    ~Room() = default;

    Room(int id, vector<int> adjoining_rooms_id, int iron, int gold, int gems, int exp)
        : id(id), adjoining_rooms_id(move(adjoining_rooms_id)), iron(iron), gold(gold), gems(gems), exp(exp) {}
    
    
};

struct Action {
    TypeAction type;
    int target_room;
    Resource resource;
     
    Action() {}
    ~Action() = default;
};


class Logger;

class VisitedRoom {
private:
    Room room;
    bool resources_were_collected = false;
    vector<Resource> resources_that_collected;

public:
    VisitedRoom(Room room) : room(room) {};
    VisitedRoom() {};
    ~VisitedRoom() = default;

    Room& getRoom() { return room; }
    const Room& getRoom() const { return room; }
    bool getResourcesWereCollected() { return resources_were_collected; }
    void setResourcesWereCollected() { resources_were_collected = true; }
    vector<Resource> getResourcesThatCollected() const { return resources_that_collected; }
    void addResourcesThatCollected(Resource res) { resources_that_collected.push_back(res); }
};

class Player {
public:
    int current_room;
    int food;
    Resource target_resource;

    map<int, VisitedRoom> visited_rooms;
    map<int, vector<int>> adjacent_rooms;
    
    int collected_iron, collected_gold, collected_gems, collected_exp;

    Player() : current_room(0), food(0), collected_iron(0), collected_gold(0), collected_gems(0), collected_exp(0) {}
    ~Player() = default;

    
    void init(int start_food, Resource target, Room& room_data) {
        visited_rooms.clear();
        adjacent_rooms.clear();
        current_room = 0;
        enterRoom(room_data);
        visited_rooms[0].setResourcesWereCollected();
        food = start_food;
        target_resource = target;
        collected_iron = collected_gold = collected_gems = collected_exp = 0;
    }

    int resourceValue(Resource res) const {
        int base = 0;
        switch (res) {
            case Resource::IRON: base = VALUE_IRON;  break;
            case Resource::GOLD: base = VALUE_GOLD;  break;
            case Resource::GEMS: base = VALUE_GEMS;  break;
            case Resource::EXP:  base = VALUE_EXP;   break;
            case Resource::NONE:  base = VALUE_NONE; break;
        }

        return (res == this->target_resource) ? base * 2 : base;
    }

    string resourceName(Resource res) const {
        switch (res) {
            case Resource::IRON: return "iron";
            case Resource::GOLD: return "gold";
            case Resource::GEMS: return "gems";
            case Resource::EXP:  return "exp";
            case Resource::NONE: return "none resourse";
        }

        return "unknown";
    }

    void enterRoom(const Room& room_data) {
        int id = room_data.id;
        
        adjacent_rooms.erase(id);
        visited_rooms[id] = room_data;
        
        for (int neighbor : room_data.adjoining_rooms_id) {
            if (visited_rooms.find(neighbor) == visited_rooms.end() &&
                adjacent_rooms.find(neighbor) == adjacent_rooms.end()) {
                adjacent_rooms[neighbor];
            }
        }
    }

    void learnAdjacentNeighbors(int room_id, const vector<int>& neighbors) {
        if (adjacent_rooms.find(room_id) != adjacent_rooms.end()) {
            adjacent_rooms[room_id] = neighbors;
        }
    }

    bool moveToRoom(int room_id) {
        if (food <= 0 && current_room != 0) return false;
        food--;
        current_room = room_id;
        return true;
    }

    bool isVisited(int room_id) const {
        return visited_rooms.find(room_id) != visited_rooms.end();
    }

    bool collectResource(Resource res) {
        auto it = visited_rooms.find(current_room);
        if (it == visited_rooms.end()) return false;
        
        VisitedRoom& v_room = it->second;

        if (res == Resource::NONE){
            v_room.setResourcesWereCollected();
            v_room.addResourcesThatCollected(Resource::GOLD);
            v_room.addResourcesThatCollected(Resource::EXP);
            v_room.addResourcesThatCollected(Resource::GEMS);
            v_room.addResourcesThatCollected(Resource::IRON);
            return true;
        } 

        
        int* amount = nullptr;
        
        switch (res) {
            case Resource::IRON: amount = &v_room.getRoom().iron; break;
            case Resource::GOLD: amount = &v_room.getRoom().gold; break;
            case Resource::GEMS: amount = &v_room.getRoom().gems; break;
            case Resource::EXP:  amount = &v_room.getRoom().exp;  break;
        }
        
        if (*amount <= 0) return false;

        if (v_room.getResourcesWereCollected()) {
            if (food <= 0 && current_room != 0) return false;
            if (current_room != 0) food--;
        } else {
            v_room.setResourcesWereCollected();
        }
        
        switch (res) {
            case Resource::IRON:
                collected_iron += *amount; 
                v_room.addResourcesThatCollected(Resource::IRON);
                break;
            case Resource::GOLD:
                collected_gold += *amount;
                v_room.addResourcesThatCollected(Resource::GOLD);
                break;
            case Resource::GEMS: 
                collected_gems += *amount;
                v_room.addResourcesThatCollected(Resource::GEMS);
                break;
            case Resource::EXP:
                collected_exp += *amount;
                v_room.addResourcesThatCollected(Resource::EXP);
                break;
        }

        *amount = 0;
        
        return true;
    }

    Resource best_resource_in_room(int room_id) {
        auto iterator = visited_rooms.find(room_id);
        if (iterator == visited_rooms.end()) return Resource::IRON;
        
        VisitedRoom& v_room = iterator->second;
        
        vector<pair<int, Resource>> sorted;
        if (v_room.getRoom().iron > 0) {
            int cost_one = resourceValue(Resource::IRON);
            int cost_all = cost_one * v_room.getRoom().iron;
            sorted.push_back({ cost_all, Resource::IRON});
        }
        if (v_room.getRoom().gold > 0) {
            int cost_one = resourceValue(Resource::GOLD);
            int cost_all = cost_one * v_room.getRoom().gold;
            sorted.push_back({ cost_all, Resource::GOLD});
        }
        if (v_room.getRoom().gems > 0) {
            int cost_one = resourceValue(Resource::GEMS);
            int cost_all = cost_one * v_room.getRoom().gems;
            sorted.push_back({ cost_all, Resource::GEMS});
        }
        if (v_room.getRoom().exp  > 0) {
            int cost_one = resourceValue(Resource::EXP);
            int cost_all = cost_one * v_room.getRoom().exp;
            sorted.push_back({ cost_all, Resource::EXP});
        }
            
        if (sorted.empty()) { return Resource::NONE; }

        sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
        
        return sorted[0].second;
    }

    int totalvalue() const {
        return collected_iron * resourceValue(Resource::IRON) +
               collected_gold * resourceValue(Resource::GOLD) +
               collected_gems * resourceValue(Resource::GEMS) +
               collected_exp  * resourceValue(Resource::EXP);
    }

    vector<int> findPathToStart() const {
        queue<int> queue;
        map<int, int> parent;
        map<int, bool> used;
        
        queue.push(current_room);
        used[current_room] = true;
        parent[current_room] = -1;
        
        while (!queue.empty()) {
            int v = queue.front();
            queue.pop();
            
            if (v == 0) {
                vector<int> path;
                for (int i = 0; i != -1; i = parent[i]) path.push_back(i);
                reverse(path.begin(), path.end());
                path.erase(path.begin());
                return path;
            }
            
            auto it_visit = visited_rooms.find(v);
            if (it_visit == visited_rooms.end()) continue;
            
            vector<int> neighbors = it_visit->second.getRoom().adjoining_rooms_id;
            sort(neighbors.begin(), neighbors.end());
            
            for (int neighbor : neighbors) {
                if (!used[neighbor] && isVisited(neighbor)) {
                    used[neighbor] = true;
                    parent[neighbor] = v;
                    queue.push(neighbor);
                }
            }
        }
        return {};
    }

    vector<int> findNearestUnvisited() const {
        queue<int> q;
        map<int, int> parent;
        map<int, bool> used;
        
        q.push(current_room);
        used[current_room] = true;
        parent[current_room] = -1;
        
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            
            auto it_visit = visited_rooms.find(v);
            if (it_visit == visited_rooms.end()) continue;
            
            vector<int> neighbors = it_visit->second.getRoom().adjoining_rooms_id;
            sort(neighbors.begin(), neighbors.end());
            
            for (int to : neighbors) {
                if (adjacent_rooms.find(to) != adjacent_rooms.end()) {
                    parent[to] = v;
                    vector<int> path;
                    for (int cur = to; cur != -1; cur = parent[cur]) {
                        path.push_back(cur);
                    }
                    reverse(path.begin(), path.end());
                    path.erase(path.begin());
                    return path;
                }
                
                if (!used[to] && isVisited(to)) {
                    used[to] = true;
                    parent[to] = v;
                    q.push(to);
                }
            }
        }
        return {};
    }
};

class IStrategyBot {
public:
    virtual ~IStrategyBot() = default;
    
    virtual Action nextAction(Player& player, Logger& logger) = 0;
};

class Logger {
private:
    ofstream result_file;

public:
    Logger() {};
    ~Logger() {
        if (result_file.is_open()) {
            result_file.flush();
            result_file.close();
        }
    };

    bool init(const string& output_file) {
        result_file.open(output_file);
        if (!result_file.is_open()) {
            return false;
        }
        return true;
    }

    void logGo(int room) { result_file << "go " << room << endl; }

    void logCollect(const string& resource) { result_file << "collect " << resource << endl; }

    void logState(const Player& player) {
        player;
        string resource;
        const int number_room = player.current_room;
        
        if (number_room == 0) return;

        vector<Resource> collected_res =  player.visited_rooms.at(number_room).getResourcesThatCollected();

        bool find_iron = find(collected_res.begin(), collected_res.end(), Resource::IRON) != collected_res.end();
        bool find_gold = find(collected_res.begin(), collected_res.end(), Resource::GOLD) != collected_res.end();
        bool find_gems = find(collected_res.begin(), collected_res.end(), Resource::GEMS) != collected_res.end();
        bool find_exp = find(collected_res.begin(), collected_res.end(), Resource::EXP) != collected_res.end();

        resource += find_iron ? "_" : to_string(player.visited_rooms.at(number_room).getRoom().iron);
        resource += " ";

        resource += find_gold ? "_" : to_string(player.visited_rooms.at(number_room).getRoom().gold);
        resource += " ";
        
        resource += find_gems ? "_" : to_string(player.visited_rooms.at(number_room).getRoom().gems);
        resource += " ";
        
        resource += find_exp ? "_" : to_string(player.visited_rooms.at(number_room).getRoom().exp);

        result_file << "state" << " " << player.current_room << " " << resource << endl;
    }

    void logResult(const Player& player) { 
        result_file << "result" << " " << player.collected_iron << " " << player.collected_gold << " " << player.collected_gems << " " << player.collected_exp << " " << player.totalvalue() << endl; 
    }

    void logString(const string& str) { result_file << str << endl; }
};

class AliceBot: public IStrategyBot {
private:
    int initially_count_food;
    PhaseBot phase;
    vector<int> path_to_target;
    size_t path_index = 0;

public:
    AliceBot(int food): initially_count_food(food), phase(PhaseBot::RESEARCH) {};
    ~AliceBot() = default;

    Action nextAction(Player& player, Logger& logger) override {
        if(player.food == 0) {
            Action act;
            act.type = TypeAction::FINISH;
            return act;
        }

        int food = player.food;
        if(food >= initially_count_food / 2 && phase == PhaseBot::RESEARCH) {
            vector<int> next_room = player.findNearestUnvisited();
            if (next_room.empty()) {
                logger.logString("All rooms have been visited and more than half of the food has been consumed. Moving on to the return phase!");
                path_index = 0;
                path_to_target = player.findPathToStart(); 
                phase = PhaseBot::RETURN;
            }
        }
        if (food <= initially_count_food / 2) {
            if(phase != PhaseBot::RETURN) {
                path_index = 0;
                path_to_target = player.findPathToStart(); 
                phase = PhaseBot::RETURN;
            }
            if(!player.visited_rooms[player.current_room].getResourcesWereCollected()) {
                phase = PhaseBot::RESEARCH;
            }
        }

        if (phase == PhaseBot::RESEARCH) {
            return researchAction(player);
        }

        return returnAction(player);
    }


private:
    Action researchAction(Player& player) {
        Action act;
        Resource best = player.best_resource_in_room(player.current_room);
        if (!player.visited_rooms[player.current_room].getResourcesWereCollected() && player.current_room != 0) {
            act.type = TypeAction::COLLECT;
            act.resource = best;
            return act;
        }

        if (path_to_target.empty()) {
            vector<int> path_nearest_unvisited = player.findNearestUnvisited();
            path_to_target = path_nearest_unvisited;
            path_index = 0;
            act.type = TypeAction::GO;
            act.target_room = path_to_target[path_index];
            path_index++;
            return act;
        }

        if (path_index < path_to_target.size()) {
            int next = path_to_target[path_index];
            path_index++;
            act.type = TypeAction::GO;
            act.target_room = next;
            return act;
        }
        path_to_target.clear();
        path_index = 0;

        
        return act;
    }

    Action returnAction(Player& player) {
        Action act;
        size_t dist = path_to_target.size();

        
        if(path_index < dist) {
            if (dist - path_index < player.food) {
                Resource best = player.best_resource_in_room(player.current_room);
                if (best != Resource::NONE) {
                    act.type = TypeAction::COLLECT;
                    act.resource = best;
                    return act;
                }
            }
            int next = path_to_target[path_index];
            path_index++;
            act.type = TypeAction::GO;
            act.target_room = next;
            return act;
        }

        act.type = TypeAction::FINISH;
        return act;
        
    }
};

class Map {
public:

    Map() {};
    ~Map() = default;

    bool readDataFromFile(string file_name, Logger& logger) {
        
        ifstream file(file_name);
        if (!file.is_open()) {
            cerr << "Cannot open file" << endl;
            return false;
        }

        if (!this->parseCountRooms(file, logger) ) { file.close(); return false;}
        if (!this->parseInfoOfRooms(file, logger)) { file.close(); return false;}
        if (!this->parseFoodAndTargetResource(file, logger)) { file.close(); return false;}
        
        file.close();
        return true;
    }

    Room* getRoom(int id) {
        auto iterator = rooms.find(id);
        if (iterator == rooms.end()) {
            cerr << "ERROR: room with this id doesn't exist" << endl;
            return nullptr;
        }

        return &iterator->second;
    }

    int getFood() {
        return this->food;
    }

    Resource getTargetResource() {
        return this->target_resource;
    }

private:
    int N;
    map<int, Room> rooms;
    int food;
    Resource target_resource;
    
    bool parseResource(const string& str, Resource& res) {
        if (str == "iron") { res = Resource::IRON; return true; }
        if (str == "gold") { res = Resource::GOLD; return true; }
        if (str == "gems") { res = Resource::GEMS; return true; }
        if (str == "exp")  { res = Resource::EXP;  return true; }
        res = Resource::NONE;
        return false;
    }

    string trim(const string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    bool checkStrIsPosNumber(const string& str) {
        if(str.empty()) return false;
        for(unsigned char i : str) {
            if(!isdigit(i)) return false;
        }
        return true;
    }

    bool getNumberFromStr(const string& line, int& N, bool check_limits=false, int min_lim=0, int max_lim=255) {
        string new_line = trim(line);

        if(!checkStrIsPosNumber(new_line)) {
            return false;
        }

        N = stoi(new_line);

        if (check_limits && (N < min_lim || N > max_lim)) {
            return false;
        }
        return true;
    }

    bool parsStrByCharacters(const string& str, vector<int>& result_list, const char character) {
        stringstream stream(str);
        string item;
        
        while (getline(stream, item, character)) {
            
            if (!checkStrIsPosNumber(item)) {
                return false;
            }
            
            result_list.push_back(stoi(item));
        }
        
        return true;
    }

    bool getAdjoiningRoomsFromStr(const string& str, vector<int>& neighbors, const int& maxN) {   
        if (str.empty()) return true;
        if (!parsStrByCharacters(str, neighbors, ',')) {
            return false;
        }

        for (int i : neighbors) {
            if (i < 0 || i > maxN) return false;
        }

        return true;
    }

    vector<string> splitBySpaces(const string& str) {
        vector<string> tokens;
        stringstream stream(str);
        string token;

        while (getline(stream, token, ' ')) {
            tokens.push_back(token);
        }

        return tokens;
    }

    bool parseCountRooms(ifstream& file_input, Logger& loger) {
        string line;

        if (!getline(file_input, line)) {
            loger.logString("");
            return false;
        }
        
        if(!getNumberFromStr(line, this->N, true, 1, 255)) {
            loger.logString(line);
            return false;
        }

        return true;
    }

    bool parseInfoOfRooms(ifstream& file_input, Logger& loger) {

        string line;
        for (size_t i = 0; i <= N; i++) {
            if (!getline(file_input, line)) {
                loger.logString("Missing room data");
                return false;
            }

            vector<string> parts = splitBySpaces(line);

            if (parts.size() == 6) {
                Room new_room;
                bool id_corrected = getNumberFromStr(parts[0], new_room.id, true, 0, N);
                bool neighbors_corrected = getAdjoiningRoomsFromStr(parts[1], new_room.adjoining_rooms_id, N);
                bool iron_corrected = getNumberFromStr(parts[2], new_room.iron, true, 0, 255);
                bool gold_corrected = getNumberFromStr(parts[3], new_room.gold, true, 0, 255);
                bool gems_corrected = getNumberFromStr(parts[4], new_room.gems, true, 0, 255);
                bool exp_corrected = getNumberFromStr(parts[5], new_room.exp, true, 0, 255);
                if (!(id_corrected && neighbors_corrected && iron_corrected && gold_corrected && gems_corrected && exp_corrected)) {
                    loger.logString(line);
                    return false;
                }
                this->rooms[new_room.id] = new_room;
            } else {
                loger.logString(line);
                return false;
            }
        }

        return true;
    }

    bool parseFoodAndTargetResource(ifstream& file_input, Logger& logger) {
        string line;
        if(!getline(file_input, line)) {
            logger.logString("Missing last string with food");
            return false;
        }

        vector<string> parts = splitBySpaces(line);
        if (parts.size() == 2) {
            
            bool food_corrected = getNumberFromStr(parts[0], this->food, true, 2, 255);
            bool resource_price_corrected = parseResource(parts[1], this->target_resource);

            if (!(resource_price_corrected && food_corrected)) {
                logger.logString(line);
                return false;
            }
        } else {
            logger.logString(line);
            return false;
        }

        return true;
    }
};

class Game {
public:
    Logger logger;
    Map map;
    Player player;
    IStrategyBot* bot = nullptr;    

public:
    Game() {};
    ~Game() = default;

    void startGameBot() {
        if (!bot) {
            logger.logString("ERROR: startGameBot - bot dont exist");
            return;
        }

        bool game_continues = true;
        while(game_continues) {
            
            Action act = bot->nextAction(player, logger);

            switch (act.type) {
                case TypeAction::GO:
                    if(!goToRoom(act.target_room)) game_continues = false;
                    
                    break;
                case TypeAction::COLLECT:
                    if(!collectResource(act.resource)) game_continues = false;
                    
                    break;
                case TypeAction::FINISH:
                    game_continues = false;
                    logger.logResult(player);
                    break;
            }
        }
    }

    void setBot(IStrategyBot* strategy) {
        bot = strategy;
    }

    bool goToRoom(int room_id) {
        if (gameStatus() != StageGame::CONTINUING) return false;
        if (player.current_room == 0 and player.food == 0) {
            return true;
        }

        
        if (!player.moveToRoom(room_id)) {
            logger.logString("ERROR: moveToRoom (return false)");
            return false;
        }
        
        if (!player.isVisited(room_id)) {
            Room* room_data = map.getRoom(room_id);
            if (room_data) {
                player.enterRoom(*room_data);
                
                for (auto& p : player.adjacent_rooms) {
                    int seenRoomId = p.first;
                    if (p.second.empty()) {
                        Room* room = map.getRoom(seenRoomId);
                        if (room) {
                            player.learnAdjacentNeighbors(seenRoomId, room->adjoining_rooms_id);
                        }
                    }
                }
            }
        }
        
        logger.logGo(room_id);
        logger.logState(player);

        return true;
    }

    bool init(const string& input_filename, const string& output_file="result.txt") {

        if (!map.readDataFromFile(input_filename, logger)) {
            return false;
        }
        
        if (!logger.init(output_file)) {
            return false;
        }
        
        player.init(map.getFood(), map.getTargetResource(), *map.getRoom(0));
        

        return true;
    }

    bool collectBestResource() {
        if (gameStatus() != StageGame::CONTINUING) return false;
        
        Resource best = player.best_resource_in_room(player.current_room);
        return collectResource(best);
    }

    bool collectResource(const Resource res) {
        if(player.current_room == 0) return true;
        bool result = player.collectResource(res);
        logger.logCollect(player.resourceName(res));
        logger.logState(player);
        return result;
    }

    StageGame gameStatus() {
        if (playerIsDeath()) return StageGame::LOSS;
        if (playerIsWin()) return StageGame::WIN;
        return StageGame::CONTINUING;
    }

    bool playerIsWin() {
        if (player.food <= 0 && player.current_room == 0) {
            return true;
        }
        return false;
    }

    bool playerIsDeath() {
        if (player.food <= 0 && player.current_room != 0) {
            return true;
        }
        return false;
    }

};


