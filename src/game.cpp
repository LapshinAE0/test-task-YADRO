#include "game.hpp"


void Game::startGameBot() {
    if (!bot) {
        logger.logString("ERROR: startGameBot - bot dont exist");
        return;
    }

    bool game_continues = true;
    while (game_continues) {
        Action act = bot->nextAction(player, logger);

        switch (act.type) {
            case TypeAction::GO:
                if (!goToRoom(act.target_room)) game_continues = false;
                break;
            case TypeAction::COLLECT:
                if (!collectResource(act.resource)) game_continues = false;
                break;
            case TypeAction::FINISH:
                game_continues = false;
                logger.logResult(player);
                break;
        }
    }
}

void Game::setBot(IStrategyBot* strategy) {
    bot = strategy;
}

bool Game::goToRoom(int room_id) {
    if (gameStatus() != StageGame::CONTINUING) return false;
    if (player.current_room == 0 && player.food == 0) {
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

bool Game::init(const std::string& input_filename, const std::string& output_file) {
    if (!logger.init(output_file)) return false;
    if (!map.readDataFromFile(input_filename, logger)) return false;
    player.init(map.getFood(), map.getTargetResource(), *map.getRoom(0));
    return true;
}

bool Game::collectBestResource() {
    if (gameStatus() != StageGame::CONTINUING) return false;
    Resource best = player.best_resource_in_room(player.current_room);
    return collectResource(best);
}

bool Game::collectResource(Resource res) {
    if (player.current_room == 0) return true;
    bool result = player.collectResource(res);
    logger.logCollect(player.resourceName(res));
    logger.logState(player);
    return result;
}

StageGame Game::gameStatus() {
    if (playerIsDeath()) return StageGame::LOSS;
    if (playerIsWin()) return StageGame::WIN;
    return StageGame::CONTINUING;
}

bool Game::playerIsWin() {
    return (player.food <= 0 && player.current_room == 0);
}

bool Game::playerIsDeath() {
    return (player.food <= 0 && player.current_room != 0);
}
