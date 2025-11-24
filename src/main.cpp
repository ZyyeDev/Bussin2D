#include <iostream>
#include <lua.hpp>
#include <chrono>
#include "variables.h"
#include "bindings.h"
#include "core/common.h"

const double PHYSICS_DT = 1.0 / 60.0;

double getCurrentTime(){
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

void callLuaCallback(lua_State* L, const char* name, double dt = -1.0){
    lua_getglobal(L, "buss");
    if (lua_isnil(L, -1)){
        lua_pop(L, 1);
        return;
    }

    lua_getfield(L, -1, name);
    if (lua_isfunction(L, -1)){

        if (dt >= 0.0){
            lua_pushnumber(L, dt);
        }
        int argCount = 0;
        if (dt >= 0.0) argCount = 1;

        if (lua_pcall(L,argCount,0,0) != LUA_OK){
            const char* error = lua_tostring(L,-1);
            std::cerr << "Lua error in buss. " << name << ": " << error << std::endl;
            lua_pop(L,1);
        }
    }else{
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

void updateInput(){
    if (g_keyboard) g_keyboard->update();
    if (g_mouse) g_mouse->update();
}

void handleEvent(const SDL_Event& event){
    if (g_keyboard) g_keyboard->handleEvent(event);
    if (g_mouse) g_mouse->handleEvent(event);
}

void pullEventsWithInput(){
    if (!g_window) return;

    updateInput();

    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT){
            g_window->isRunning = false;
        }
        if (event.type == SDL_WINDOWEVENT){
            if (event.window.event == SDL_WINDOWEVENT_RESIZED){

            }
        }
        handleEvent(event);
    }
}

int main(int argc, char* argv[]){
    std::cout << "we bussin" << std::endl;

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    register_all_bindings(L);
    int result = luaL_dofile(L, "lua/main.lua");
    std::cout << "lua executed" << std::endl;
    if (result != LUA_OK){
        const char* error = lua_tostring(L, -1);
        std::cerr << "Lua Error: " << error << std::endl;
        lua_pop(L, 1);
        lua_close(L);
        return 1;
    }

    callLuaCallback(L, "ready");

    double lastTime = getCurrentTime();
    double physicsAccumulator = 0.0;

    // wait for the window before executing physics_step/draw
    // idk if this is the best thing, but why would you want physics_step/draw without a window
    while (!g_window){
        std::cout << "waiting for window" << std::endl;
        sleep(100);
    }

    while (g_window && g_window->isRunning){
        double currentTime = getCurrentTime();
        double dt = currentTime-lastTime;
        lastTime = currentTime;

        pullEventsWithInput();

        physicsAccumulator += dt;
        while (physicsAccumulator >= PHYSICS_DT){
            callLuaCallback(L, "physics_step", PHYSICS_DT);
            physicsAccumulator -= PHYSICS_DT;
        }
        g_window->clear(0, 0, 0);
        callLuaCallback(L, "draw");
        g_window->present();
    }

    std::cout << "ended" << std::endl;

    lua_close(L);

    if (g_renderer) delete g_renderer;
    if (g_window) delete g_window;
    if (g_keyboard) delete g_keyboard;
    if (g_mouse) delete g_mouse;
    if (g_audio) delete g_audio;

    return 0;
}