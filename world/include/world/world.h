#pragma once

#include <engine/engine.h>

class World : public Engine {
public:
    void create() override;
    void update() override;
};