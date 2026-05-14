#ifndef ACTION_H
#define ACTION_H

#include "resource.hpp"

enum class TypeAction { GO, COLLECT, FINISH };

struct Action {
    TypeAction type;
    int target_room;
    Resource resource;
};

#endif
