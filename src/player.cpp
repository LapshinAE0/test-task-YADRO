#include "player.hpp"

Player::Player() 
    : current_room(0), food(0), target_resource(Resource::IRON),
      collected_iron(0), collected_gold(0), collected_gems(0), collected_exp(0) {}

void Player::init(int start_food, Resource target, Room& room_data) {
    visited_rooms.clear();
    adjacent_rooms.clear();
    current_room = 0;
    enterRoom(room_data);
    visited_rooms[0].setResourcesWereCollected();
    food = start_food;
    target_resource = target;
    collected_iron = collected_gold = collected_gems = collected_exp = 0;
}

int Player::resourceValue(Resource res) const {
    int base = 0;
    switch (res) {
        case Resource::IRON: base = VALUE_IRON; break;
        case Resource::GOLD: base = VALUE_GOLD; break;
        case Resource::GEMS: base = VALUE_GEMS; break;
        case Resource::EXP:  base = VALUE_EXP;  break;
        case Resource::NONE: base = VALUE_NONE; break;
    }
    return (res == target_resource) ? base * 2 : base;
}

std::string Player::resourceName(Resource res) const {
    switch (res) {
        case Resource::IRON: return "iron";
        case Resource::GOLD: return "gold";
        case Resource::GEMS: return "gems";
        case Resource::EXP:  return "exp";
        case Resource::NONE: return "none resourse";
    }
    return "unknown";
}

void Player::enterRoom(const Room& room_data) {
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

void Player::learnAdjacentNeighbors(int room_id, const std::vector<int>& neighbors) {
    if (adjacent_rooms.find(room_id) != adjacent_rooms.end()) {
        adjacent_rooms[room_id] = neighbors;
    }
}

bool Player::moveToRoom(int room_id) {
    if (food <= 0 && current_room != 0) return false;
    food--;
    current_room = room_id;
    return true;
}

bool Player::isVisited(int room_id) const {
    return visited_rooms.find(room_id) != visited_rooms.end();
}

bool Player::collectResource(Resource res) {
    auto it = visited_rooms.find(current_room);
    if (it == visited_rooms.end()) return false;
    
    VisitedRoom& v_room = it->second;

    if (res == Resource::NONE) {
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
        default: return false;
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
        default: break;
    }

    *amount = 0;
    return true;
}

Resource Player::best_resource_in_room(int room_id) {
    auto iterator = visited_rooms.find(room_id);
    if (iterator == visited_rooms.end()) return Resource::IRON;
    
    VisitedRoom& v_room = iterator->second;
    
    std::vector<std::pair<int, Resource>> sorted;
    if (v_room.getRoom().iron > 0) {
        int cost_all = resourceValue(Resource::IRON) * v_room.getRoom().iron;
        sorted.push_back({cost_all, Resource::IRON});
    }
    if (v_room.getRoom().gold > 0) {
        int cost_all = resourceValue(Resource::GOLD) * v_room.getRoom().gold;
        sorted.push_back({cost_all, Resource::GOLD});
    }
    if (v_room.getRoom().gems > 0) {
        int cost_all = resourceValue(Resource::GEMS) * v_room.getRoom().gems;
        sorted.push_back({cost_all, Resource::GEMS});
    }
    if (v_room.getRoom().exp > 0) {
        int cost_all = resourceValue(Resource::EXP) * v_room.getRoom().exp;
        sorted.push_back({cost_all, Resource::EXP});
    }
        
    if (sorted.empty()) return Resource::NONE;

    std::sort(sorted.begin(), sorted.end(), 
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    return sorted[0].second;
}

int Player::totalvalue() const {
    return collected_iron * resourceValue(Resource::IRON) +
           collected_gold * resourceValue(Resource::GOLD) +
           collected_gems * resourceValue(Resource::GEMS) +
           collected_exp  * resourceValue(Resource::EXP);
}

std::vector<int> Player::findPathToStart() const {
    std::queue<int> q;
    std::map<int, int> parent;
    std::map<int, bool> used;
    
    q.push(current_room);
    used[current_room] = true;
    parent[current_room] = -1;
    
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        
        if (v == 0) {
            std::vector<int> path;
            for (int i = 0; i != -1; i = parent[i]) path.push_back(i);
            std::reverse(path.begin(), path.end());
            path.erase(path.begin());
            return path;
        }
        
        auto it_visit = visited_rooms.find(v);
        if (it_visit == visited_rooms.end()) continue;
        
        std::vector<int> neighbors = it_visit->second.getRoom().adjoining_rooms_id;
        std::sort(neighbors.begin(), neighbors.end());
        
        for (int neighbor : neighbors) {
            if (!used[neighbor] && isVisited(neighbor)) {
                used[neighbor] = true;
                parent[neighbor] = v;
                q.push(neighbor);
            }
        }
    }
    return {};
}

std::vector<int> Player::findNearestUnvisited() const {
    std::queue<int> q;
    std::map<int, int> parent;
    std::map<int, bool> used;
    
    q.push(current_room);
    used[current_room] = true;
    parent[current_room] = -1;
    
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        
        auto it_visit = visited_rooms.find(v);
        if (it_visit == visited_rooms.end()) continue;
        
        std::vector<int> neighbors = it_visit->second.getRoom().adjoining_rooms_id;
        std::sort(neighbors.begin(), neighbors.end());
        
        for (int to : neighbors) {
            if (adjacent_rooms.find(to) != adjacent_rooms.end()) {
                parent[to] = v;
                std::vector<int> path;
                for (int cur = to; cur != -1; cur = parent[cur]) {
                    path.push_back(cur);
                }
                std::reverse(path.begin(), path.end());
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
