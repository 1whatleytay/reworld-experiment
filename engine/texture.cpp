#include <engine/texture.h>

#include <engine/engine.h>

#include <lodepng.h>

void Texture::getData(float &x1, float &y1, float &x2, float &y2) {
    float topX = static_cast<float>(x) / Engine::textureWidth;
    float topY = static_cast<float>(y) / Engine::textureHeight;
    float bottomX = static_cast<float>(x + width) / Engine::textureWidth;
    float bottomY = static_cast<float>(y + height) / Engine::textureHeight;

    x1 = flipX ? bottomX : topX;
    y1 = flipY ? bottomY : topY;
    x2 = flipX ? topX : bottomX;
    y2 = flipY ? topY : bottomY;
}

void Texture::modify(const std::vector<uint8_t> &data) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
}

Texture Texture::flipVertically() {
    Texture newTexture = *this;
    newTexture.flipY = !newTexture.flipY;
    return newTexture;
}

Texture Texture::flipHorizontally() {
    Texture newTexture = *this;
    newTexture.flipX = !newTexture.flipX;
    return newTexture;
}

Texture Texture::sample(uint32_t startX, uint32_t startY, uint32_t fullWidth, uint32_t fullHeight) {
    Texture newTexture = *this;

    newTexture.x = this->x + startX;
    newTexture.y = this->y + startY;
    newTexture.width = fullWidth;
    newTexture.height = fullHeight;
    newTexture.child = true;

    return newTexture;
}

std::vector<Texture> Texture::split(uint32_t numOfTextureX, uint32_t numOfTextureY) {
    std::vector<Texture> result;
    result.reserve(numOfTextureX * numOfTextureY);

    uint32_t subTextureWidth = width / numOfTextureX;
    uint32_t subTextureHeight = height / numOfTextureY;

    assert(width % numOfTextureX == 0);
    assert(height % numOfTextureY == 0);

    // split left to right, top to bottom
    for (uint32_t a = 0; a < numOfTextureY; a++) {
        for (uint32_t b = 0; b < numOfTextureX; b++) {
            result.push_back(sample(b * subTextureWidth, a * subTextureHeight, subTextureWidth, subTextureHeight));
        }
    }

    return result;
}

Texture::Texture() = default;

Texture::Texture(Engine *engine, const std::string &path) : engine(engine) {
    std::vector<uint8_t> data;
    lodepng::decode(data, width, height, path);

    engine->registerTexture(*this, x, y);

    modify(data);
}

Texture::Texture(Engine *engine, uint32_t width, uint32_t height) : engine(engine), width(width), height(height) {
    engine->registerTexture(*this, x, y);
}

Texture::Texture(Engine *engine, uint32_t width, uint32_t height, const std::vector<uint8_t> &data)
    : Texture(engine, width, height) {
    modify(data);
}
