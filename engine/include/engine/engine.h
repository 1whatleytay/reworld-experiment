#pragma once

#include <engine/events.h>
#include <engine/texture.h>

#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <map>
#include <array>
#include <vector>
#include <fstream>

class Thing;
class Texture;

class Engine {
    static constexpr uint32_t maxThings = 500;
    static constexpr uint32_t textureWidth = 500;
    static constexpr uint32_t textureHeight = 500;

    GLFWwindow *window = nullptr;

    GLuint program = 0;
    GLuint globalBuffer = 0;
    GLuint globalTexture = 0;
    GLuint vao = 0;

    GLint uniScreenSize = -1;
    GLint uniOffset = -1;

    std::map<Key, bool> keyStates;
    std::map<Button, bool> buttonStates;

    std::array<bool, maxThings> thingAllocations = { };
    std::array<bool, textureWidth * textureHeight> textureAllocations = { };
    std::vector<Thing *> drawOrder;

    void clickEvent(int button, int action);
    void cursorEvent(float x, float y);
    void keyEvent(int key, int action);

    static void passClickEvent(GLFWwindow *window, int button, int action, int mods);
    static void passCursorEvent(GLFWwindow *window, double x, double y);
    static void passKeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods);

    static GLuint createShader(const std::string &path, GLenum type);
    static GLuint createProgram(GLuint vertex, GLuint fragment);

    bool init();
    void loop();
    void close();

    void allocateTexture(uint32_t width, uint32_t height, uint32_t &x, uint32_t &y);

    uint32_t registerThing(Thing *thing);
    void registerTexture(Texture &texture, uint32_t &x, uint32_t &y);
    void removeThing(Thing *thing);

    friend class Thing;
    friend class Texture;
protected:
    virtual void key(Key key, Action action);
    virtual void mouse(float x, float y);
    virtual void click(Button button, Action action);

    virtual void create() = 0;
    virtual void update() = 0;

public:
    static constexpr uint32_t windowWidth = 800;
    static constexpr uint32_t windowHeight = 600;

    float offsetX = 0, offsetY = 0;

    float backgroundR = 0, backgroundG = 0, backgroundB = 0;

    bool getKeyState(Key code);
    void getMousePosition(float &x, float &y);
    bool getButtonState(Button button);

    std::map<std::string, Texture> textureCache;
    Texture loadTexture(const std::string &path);
    Texture loadTexture(const std::string &name, const std::function<Texture()> &create);

    void start();
};
