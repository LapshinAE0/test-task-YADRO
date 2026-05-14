#ifndef VISITED_ROOM_H
#define VISITED_ROOM_H

#include <vector>
#include "room.hpp"
#include "resource.hpp"

class VisitedRoom {
private:
    Room room;
    bool resources_were_collected = false;
    std::vector<Resource> resources_that_collected;

public:
    VisitedRoom() = default;
    VisitedRoom(Room room) : room(std::move(room)) {}
    ~VisitedRoom() = default;

    Room& getRoom() { return room; }
    const Room& getRoom() const { return room; }
    bool getResourcesWereCollected() const { return resources_were_collected; }
    void setResourcesWereCollected() { resources_were_collected = true; }
    std::vector<Resource> getResourcesThatCollected() const { return resources_that_collected; }
    void addResourcesThatCollected(Resource res) { resources_that_collected.push_back(res); }
};

#endif
