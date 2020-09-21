#pragma once

#include <vector>

class Engine;

class Texture {
    Engine *engine = nullptr;

    bool child = false;
    bool flipX = false, flipY = false;

    uint32_t x = 0, y = 0;
    uint32_t width = 0, height = 0;

    void getData(float &x1, float &y1, float &x2, float &y2);

    friend class Thing;
    friend class Engine;

public:
    void modify(const std::vector<uint8_t> &data);

    Texture flipVertically();
    Texture flipHorizontally();
    Texture sample(uint32_t startX, uint32_t startY, uint32_t fullWidth, uint32_t fullHeight);
    std::vector<Texture> split(uint32_t numOfTextureX, uint32_t numOfTextureY);

    Texture();
    Texture(Engine *engine, const std::string &path);
    Texture(Engine *engine, uint32_t width, uint32_t height);
    Texture(Engine *engine, uint32_t width, uint32_t height, const std::vector<uint8_t> &data);
};
