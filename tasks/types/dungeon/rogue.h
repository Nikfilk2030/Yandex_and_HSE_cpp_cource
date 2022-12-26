#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <set>

#include "dungeon.h"

/// TODO завести мапу Room*_to_num (num++)

struct MyAmmo {
    bool RoomIsVisited(Room* room) {
        for (size_t i = 0; i < visited_rooms.size(); ++i) {
            if (visited_rooms[i] == room) {
                return true;
            }
        }
        return false;
    }

    bool found_final_room = false;
    Room* final_room = nullptr;
    std::vector<Room*> visited_rooms;
    std::vector<std::string> current_keys;
};

void DFS(Room* current_room, MyAmmo& ammo) {
    if (current_room->IsFinal()) {
        ammo.found_final_room = true;
        ammo.final_room = current_room;
        return;
    }
    if (ammo.found_final_room) {
        return;
    }
    // пополняем список ключей:
    for (size_t i = 0; i < current_room->NumKeys(); ++i) {
        ammo.current_keys.emplace_back(current_room->GetKey(i));
    }
    // походим во все двери:
    for (size_t i = 0; i < current_room->NumDoors(); ++i) {
        Door* next_door = current_room->GetDoor(i);
        for (const auto& key : ammo.current_keys) {
            next_door->TryOpen(key);
        }
        if (next_door->IsOpen()) {
            Room* next_room = next_door->GoThrough();
            if (ammo.RoomIsVisited(next_room)) {
                continue;
            }
            ammo.visited_rooms.emplace_back(next_room);
            DFS(next_room, ammo);
        }
    }
}

Room* FindFinalRoom(Room* current_room) {
    MyAmmo ammo;
    DFS(current_room, ammo);
    return ammo.final_room;
}
