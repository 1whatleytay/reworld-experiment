add_library(world
    include/world/state.h
    include/world/player.h
    include/world/world.h

    player.cpp
    world.cpp)
target_include_directories(world PUBLIC include)
target_link_libraries(world PUBLIC engine)
