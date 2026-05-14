#ifndef PLAYER_H
#define PLAYER_H

#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include "resource.hpp"
#include "room.hpp"
#include "visited_room.hpp"

class Player {
public:
    int current_room;
    int food;
    Resource target_resource;

    std::map<int, VisitedRoom> visited_rooms;
    std::map<int, std::vector<int>> adjacent_rooms;
    
    int collected_iron, collected_gold, collected_gems, collected_exp;

    Player();
    ~Player() = default;

    void init(int start_food, Resource target, Room& room_data);
    int resourceValue(Resource res) const;
    std::string resourceName(Resource res) const;
    void enterRoom(const Room& room_data);
    void learnAdjacentNeighbors(int room_id, const std::vector<int>& neighbors);
    bool moveToRoom(int room_id);
    bool isVisited(int room_id) const;
    bool collectResource(Resource res);
    Resource best_resource_in_room(int room_id);
    int totalvalue() const;
    std::vector<int> findPathToStart() const;
    std::vector<int> findNearestUnvisited() const;
};

#endif
