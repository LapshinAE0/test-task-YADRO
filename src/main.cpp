#include <iostream>
#include "game.hpp"
#include "alice_bot.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <in.txt>" << std::endl;
        return 1;
    }

    Game game;
    if (!game.init(argv[1])) {
        return 1;
    }

    AliceBot bot(game.map.getFood());

    game.setBot(&bot);

    game.startGameBot();

    return 0;
}
