#pragma once

#include <engine/thing.h>

class Player : public Thing {
public:
    Player(Engine *engine, float x, float y, float width, float height);
};