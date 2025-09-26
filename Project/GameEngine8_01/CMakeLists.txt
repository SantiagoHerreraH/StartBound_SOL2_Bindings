cmake_minimum_required(VERSION 3.20)
project(GameEngine8_01)

set(EXE_NAME GameEngine8_01)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch LUA
# ++++++++++
FetchContent_Declare(
    lua
    URL https://github.com/marovira/lua/archive/refs/tags/5.4.4.tar.gz
)
FetchContent_MakeAvailable(lua)

# Fetch SOL2
# ++++++++++
FetchContent_Declare(
    sol2
    URL https://github.com/ThePhD/sol2/archive/refs/tags/v3.3.0.tar.gz
)
FetchContent_MakeAvailable(sol2)

find_library(GDIPLUS_LIBRARY NAMES libgdiplus gdiplus)
set(GDIPLUS_LIBRARY gdiplus)

# Collect Minigin source files
file(GLOB_RECURSE MINIGIN_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Minigin/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Minigin/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Minigin/*.hpp"
)

# Collect 3rdParty headers
file(GLOB_RECURSE THIRD_PARTY_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/*.hpp"
)

set(SRC_FILES
    "src/AbstractGame.cpp"
    "src/Game.cpp"
    "src/GameDefines.h"
    "src/GameEngine.cpp"
    "src/GameWinMain.cpp"
    "src/resource.h"
    "src/LuaController.cpp"
    "src/LuaController.h"
)

# Add Minigin and 3rdParty files
list(APPEND SRC_FILES ${MINIGIN_SRC_FILES})
list(APPEND SRC_FILES ${THIRD_PARTY_SRC_FILES})

add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE /bigobj)

target_link_libraries(${PROJECT_NAME} PRIVATE sol2 lua::lua)

set(LUA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts.lua"
    # add more Lua files here if you require() them at runtime, e.g.:
    # "${CMAKE_CURRENT_SOURCE_DIR}/annotation.lua"
)

foreach(LUA_SOURCE IN LISTS LUA_SOURCES)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${LUA_SOURCE}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
        COMMENT "Copying ${LUA_SOURCE} -> $<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endforeach()
