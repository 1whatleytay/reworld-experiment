#pragma once

#include <engine/events.h>
#include <engine/texture.h>

#include <vector>

class Engine;

class Thing {
    static constexpr uint32_t vertexSize = 10 * sizeof(float);
    static constexpr uint32_t objectSize = 6 * vertexSize;

    uint32_t index;

    std::vector<float> createData();

    friend class Engine;

protected:
    Engine *engine;

    virtual void key(Key key, Action action);
    virtual void mouse(float pX, float pY);
    virtual void click(Button button, Action action);

    virtual void update();

public:
    float x = 0;
    float y = 0;

    float width = 0;
    float height = 0;

    Texture texture;

    bool isVisible = true;
    float tintR = 1, tintG = 1, tintB = 1;
    float originX = 0, originY = 0;
    float rotation = 0;

    void save();
    void remove();

    Thing(Engine *engine, float x, float y, float width, float height);
    Thing(Engine *engine, float x, float y, float width, float height, Texture texture);
    virtual ~Thing();
};
