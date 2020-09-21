#include <engine/engine.h>

#include <engine/thing.h>
#include <engine/texture.h>

GLuint Engine::createShader(const std::string &path, GLenum type) {
    GLuint shader = glCreateShader(type);

    std::ifstream sourceStream(path, std::ios::binary | std::ios::ate);
    assert(sourceStream.is_open());
    std::vector<char> source(sourceStream.tellg());
    sourceStream.seekg(0, std::ios::beg);
    sourceStream.read(source.data(), source.size());
    sourceStream.close();

    GLint sourceLength = source.size();
    char *sourceData = source.data();

    glShaderSource(shader, 1, &sourceData, &sourceLength);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    assert(status);

    return shader;
}

GLuint Engine::createProgram(GLuint vertex, GLuint fragment) {
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    assert(status);

    glDetachShader(program, vertex);
    glDetachShader(program, fragment);

    return program;
}

void Engine::allocateTexture(uint32_t width, uint32_t height, uint32_t &x, uint32_t &y) {
    for (int scanX = 0; scanX < textureWidth - width + 1; scanX++) {
        for (int scanY = 0; scanY < textureHeight - height + 1; scanY++) {
            bool isFound = false;
            for (int scanPX = 0; scanPX < width; scanPX++) {
                for (int scanPY = 0; scanPY < height; scanPY++) {
                    if (textureAllocations[(scanX + scanPX) + (scanY + scanPY) * textureWidth]) {
                        isFound = true;
                        break;
                    }
                }
                if (isFound) break;
            }
            if (!isFound) {
                x = scanX;
                y = scanY;
                return;
            }
        }
    }

    // No space left.
    assert(false);
}

uint32_t Engine::registerThing(Thing *thing) {
    uint32_t index = maxThings;
    for (size_t a = 0; a < maxThings; a++) {
        if (!thingAllocations[a]) {
            index = a;
            break;
        }
    }
    assert(index < maxThings);
    thingAllocations[index] = true;
    drawOrder.push_back(thing);
    return index;
}

void Engine::registerTexture(Texture &texture, uint32_t &x, uint32_t &y) {
    allocateTexture(texture.width, texture.height, x, y);

    for (uint32_t a = 0; a < texture.width; a++) {
        for (uint32_t b = 0; b < texture.height; b++) {
            textureAllocations[x + a + (y + b) * textureWidth] = true;
        }
    }
}

void Engine::removeThing(Thing *thing) {
    thingAllocations[thing->index] = false;

    for (size_t a = 0; a < drawOrder.size(); a++) {
        if (drawOrder[a] == thing) {
            drawOrder[a] = nullptr;
        }
    }
}

void Engine::key(Key key, Action action) { }
void Engine::mouse(float x, float y) { }
void Engine::click(Button button, Action action) { }

void Engine::clickEvent(int button, int action) {
    if (action == GLFW_REPEAT)
        return;

    auto buttonCode = static_cast<Button>(button);
    auto actionCode = static_cast<Action>(action);

    buttonStates[buttonCode] = actionCode == Action::Pressed;

    click(buttonCode, actionCode);
    for (size_t a = 0; a < drawOrder.size(); a++) {
        if (drawOrder[a]) {
            drawOrder[a]->click(buttonCode, actionCode);
        }
    }
}

void Engine::cursorEvent(float x, float y) {
    x -= offsetX;
    y -= offsetY;

    mouse(x, y);
    for (size_t a = 0; a < drawOrder.size(); a++) {
        if (drawOrder[a]) {
            drawOrder[a]->mouse(x, y);
        }
    }
}

void Engine::keyEvent(int code, int action) {
    if (action == GLFW_REPEAT)
        return;

    auto keyCode = static_cast<Key>(code);
    auto actionCode = static_cast<Action>(action);

    keyStates[keyCode] = actionCode == Action::Pressed;

    key(keyCode, actionCode);
    for (size_t a = 0; a < drawOrder.size(); a++) {
        if (drawOrder[a]) {
            drawOrder[a]->key(keyCode, actionCode);
        }
    }
}

void Engine::passClickEvent(GLFWwindow *window, int button, int action, int mods) {
    auto *engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));

    engine->clickEvent(button, action);
}

void Engine::passCursorEvent(GLFWwindow *window, double x, double y) {
    auto *engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));

    engine->cursorEvent(static_cast<float>(x), static_cast<float>(y));
}

void Engine::passKeyEvent(GLFWwindow *window, int code, int scancode, int action, int mods) {
    auto *engine = reinterpret_cast<Engine *>(glfwGetWindowUserPointer(window));

    engine->keyEvent(code, action);
}

bool Engine::init() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(windowWidth, windowHeight, "ReWorld", nullptr, nullptr);

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, passKeyEvent);
    glfwSetMouseButtonCallback(window, passClickEvent);
    glfwSetCursorPosCallback(window, passCursorEvent);

    glfwMakeContextCurrent(window);

    GLuint vertex = createShader("main.vert", GL_VERTEX_SHADER);
    GLuint fragment = createShader("main.frag", GL_FRAGMENT_SHADER);
    program = createProgram(vertex, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glUseProgram(program);
    uniScreenSize = glGetUniformLocation(program, "screenSize");
    uniOffset = glGetUniformLocation(program, "offset");
    glUniform2f(uniScreenSize, windowWidth, windowHeight);
    glUniform2f(uniOffset, 0, 0);

    glGenBuffers(1, &globalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, globalBuffer);
    glBufferData(GL_ARRAY_BUFFER, Thing::objectSize * maxThings, nullptr, GL_STATIC_READ);

    glGenTextures(1, &globalTexture);
    glBindTexture(GL_TEXTURE_2D, globalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, Thing::vertexSize, (void *)(0)); // position
    glVertexAttribPointer(1, 2, GL_FLOAT, false, Thing::vertexSize, (void *)(2 * sizeof(float))); // texCoord
    glVertexAttribPointer(2, 3, GL_FLOAT, false, Thing::vertexSize, (void *)(4 * sizeof(float))); // tint
    glVertexAttribPointer(3, 2, GL_FLOAT, false, Thing::vertexSize, (void *)(7 * sizeof(float))); // origin
    glVertexAttribPointer(4, 1, GL_FLOAT, false, Thing::vertexSize, (void *)(9 * sizeof(float))); // rotation
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    create();

    return true;
}

void Engine::loop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        update();

        for (size_t a = 0; a < drawOrder.size(); a++) {
            if (drawOrder[a]) {
                drawOrder[a]->update();
            }
        }

        for (size_t a = 0; a < drawOrder.size(); a++) {
            if (!drawOrder[a]) {
                drawOrder.erase(drawOrder.begin() + a);
                a--;
            }
        }

        glClearColor(backgroundR, backgroundG, backgroundB, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform2f(uniOffset, offsetX, offsetY);

        for (Thing *thing : drawOrder) {
            if (thing->isVisible) {
                glDrawArrays(GL_TRIANGLES, thing->index * 6, 6);
            }
        }

        glfwSwapBuffers(window);
    }
}

void Engine::close() {
    glfwDestroyWindow(window);
}

bool Engine::getKeyState(Key code) {
    auto state = keyStates.find(code);

    if (state == keyStates.end())
        return false;

    return state->second;
}

void Engine::getMousePosition(float &x, float &y) {
    double tempX, tempY;

    glfwGetCursorPos(window, &tempX, &tempY);

    x = static_cast<float>(tempX) - offsetX;
    y = static_cast<float>(tempY) - offsetY;
}

bool Engine::getButtonState(Button button) {
    auto state = buttonStates.find(button);

    if (state == buttonStates.end())
        return false;

    return state->second;
}

Texture Engine::loadTexture(const std::string &path) {
    return loadTexture(path, [&]() { return Texture(this, path); });
}

Texture Engine::loadTexture(const std::string &name, const std::function<Texture()> &create) {
    auto cached = textureCache.find(name);

    if (cached == textureCache.end()) {
        Texture texture = create();
        textureCache[name] = texture;
        return texture;
    }

    return cached->second;
}

void Engine::start() {
    if (init())
        loop();

    close();
}
