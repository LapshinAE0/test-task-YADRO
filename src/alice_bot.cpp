#include "alice_bot.hpp"
#include "player.hpp"
#include "logger.hpp"

AliceBot::AliceBot(int food) : initially_count_food(food), phase(PhaseBot::RESEARCH) {}

Action AliceBot::nextAction(Player& player, Logger& logger) {
    if (player.food == 0) {
        Action act;
        act.type = TypeAction::FINISH;
        return act;
    }

    int food = player.food;
    
    if (food > initially_count_food / 2 && phase == PhaseBot::RESEARCH) {
        std::vector<int> next_room = player.findNearestUnvisited();
        if (next_room.empty()) {
            logger.logString("All rooms have been visited and more than half of the food has been consumed. Moving on to the return phase!");
            path_index = 0;
            path_to_target = player.findPathToStart();
            phase = PhaseBot::RETURN;
        }
    }
    
    if (food <= initially_count_food / 2) {
        if (phase != PhaseBot::RETURN) {
            path_index = 0;
            path_to_target = player.findPathToStart();
            phase = PhaseBot::RETURN;
        }
        if (!player.visited_rooms[player.current_room].getResourcesWereCollected()) {
            phase = PhaseBot::RESEARCH;
        }
    }

    if (phase == PhaseBot::RESEARCH) {
        return researchAction(player);
    }

    return returnAction(player);
}

Action AliceBot::researchAction(Player& player) {
    Action act;
    Resource best = player.best_resource_in_room(player.current_room);
    
    if (!player.visited_rooms[player.current_room].getResourcesWereCollected() && player.current_room != 0) {
        act.type = TypeAction::COLLECT;
        act.resource = best;
        return act;
    }

    if (path_to_target.empty()) {
        std::vector<int> path_nearest_unvisited = player.findNearestUnvisited();
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

Action AliceBot::returnAction(Player& player) {
    Action act;
    size_t dist = path_to_target.size();

    if (path_index < dist) {
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
