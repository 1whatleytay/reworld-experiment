add_library(engine
    include/engine/events.h
    include/engine/engine.h
    include/engine/texture.h
    include/engine/thing.h

    engine.cpp
    texture.cpp
    thing.cpp)

target_include_directories(engine PUBLIC include)
target_link_libraries(engine PUBLIC lodepng glfw OpenGL::GL)
