#ifndef ROOM_H
#define ROOM_H

#include <vector>

struct Room {
    int id;
    std::vector<int> adjoining_rooms_id;
    int iron, gold, gems, exp;

    Room() : id(0), iron(0), gold(0), gems(0), exp(0) {}
    ~Room() = default;

    Room(int id, std::vector<int> adj, int iron, int gold, int gems, int exp)
        : id(id), adjoining_rooms_id(std::move(adj)),
          iron(iron), gold(gold), gems(gems), exp(exp) {}
};

#endif
