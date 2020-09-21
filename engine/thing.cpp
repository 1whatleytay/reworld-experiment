#include <engine/thing.h>

#include <engine/engine.h>
#include <engine/texture.h>

std::vector<float> Thing::createData() {
    float x1 = 0, y1 = 0;
    float x2 = 0, y2 = 0;

    texture.getData(x1, y1, x2, y2);

    return {
        x, y, x1, y1, tintR, tintG, tintB, originX, originY, rotation,
        x, y + height, x1, y2, tintR, tintG, tintB, originX, originY, rotation,
        x + width, y + height, x2, y2, tintR, tintG, tintB, originX, originY, rotation,

        x, y, x1, y1, tintR, tintG, tintB, originX, originY, rotation,
        x + width, y + height, x2, y2, tintR, tintG, tintB, originX, originY, rotation,
        x + width, y, x2, y1, tintR, tintG, tintB, originX, originY, rotation,
    };
}

void Thing::save() {
    std::vector<float> data = createData();
    glBufferSubData(GL_ARRAY_BUFFER, index * Thing::objectSize, Thing::objectSize, data.data());
}

void Thing::key(Key key, Action action) { }
void Thing::mouse(float pX, float pY) { }
void Thing::click(Button button, Action action) { }
void Thing::update() { }

void Thing::remove() {
    engine->removeThing(this);
}

Thing::Thing(Engine *engine, float x, float y, float width, float height) : Thing(engine, x, y, width, height, { }) { }

Thing::Thing(Engine *engine, float x, float y, float width, float height, Texture texture)
    : engine(engine), x(x), y(y), width(width), height(height), texture(texture), index(engine->registerThing(this)) {
    save();
}

Thing::~Thing() {
    remove();
}
