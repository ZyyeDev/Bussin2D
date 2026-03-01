#include <iostream>
#include <lua.hpp>
#include <chrono>
#include <string>
#include <filesystem>

#include "variables.h"
#include "bindings.h"
#include "modules/vfs/vfs.h"
#include "core/common.h"

#ifdef BUSSIN_EDITOR
    #include "core/build/build.h"
#endif

#ifdef _WIN32
    #include <windows.h>

    void showConsole(){
        if (AttachConsole(ATTACH_PARENT_PROCESS)){
            FILE* fpStdout = nullptr;
            freopen_s(&fpStdout, "CONOUT$", "w", stdout);
            
            FILE* fpStderr = nullptr;
            freopen_s(&fpStderr, "CONOUT$", "w", stderr);
            
            FILE* fpStdin = nullptr;
            freopen_s(&fpStdin, "CONIN$", "r", stdin);
            
            std::cout.clear();
            std::cerr.clear();
            std::cin.clear();

            std::cout << std::endl;
        }
    }
#endif

double PHYSICS_DT = 1.0 / 60.0;

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

void handleEvent(const SDL_Event& event){
    if (g_keyboard) g_keyboard->handleEvent(event);
    if (g_mouse) g_mouse->handleEvent(event);
}

void pullEventsWithInput(){
    if (!g_window) return;

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
    if (g_mouse) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        g_mouse->setX(mx);
        g_mouse->setY(my);
    }
}

int main(int argc, char* argv[]){
    #ifdef _WIN32
        showConsole();
        // ansi shit 
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;

        if (hOut != INVALID_HANDLE_VALUE)
            if (GetConsoleMode(hOut, &dwMode))
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
    #endif

    #ifdef BUSSIN_EDITOR
        if (argc > 1 && std::string(argv[1]) == "build"){
            std::string platform;
            if (argc > 2){
                platform = argv[2];
            }else{
                std::cerr << "Please specify build target";
                return 1;
            }
            return buildProject(platform);
        }
    
        bool debug = argc > 1 && std::string(argv[1]) == "debug";
    #else
        bool debug = false;
    #endif

    std::cout << "we bussin" << std::endl;
    
    std::cout << std::boolalpha;
    std::cout << "RUNNING ON VERSION " << ENGINE_VERSION << " WITH DEBUG: " << debug << std::endl;

    init_vfs();

    g_vfs->init(!debug);

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    install_vfs_loader(L);
    register_all_bindings(L);

    std::string code = g_vfs->readText("main.lua");
    if (code.empty()){
        std::cerr << "main.lua is empty!!" << std::endl;
        return 1;
    }

    lua_State* mainThread = lua_newthread(L);
    lua_setglobal(L, "__main_coroutine");

    if (luaL_loadbuffer(mainThread, code.c_str(), code.size(), "main.lua") != LUA_OK){
        std::cerr << "Load Error: " << lua_tostring(mainThread, -1) << std::endl;
        return 1;
    }

    int mainStatus = lua_resume(mainThread, 0);
    if (mainStatus != LUA_OK && mainStatus != LUA_YIELD){
        std::cerr << "Error mainStatus: " << lua_tostring(mainThread, -1) << std::endl;
        return 1;
    }

    /*int result = luaL_dofile(L, "lua/main.lua");
    std::cout << "lua executed" << std::endl;
    if (result != LUA_OK){
        const char* error = lua_tostring(L, -1);
        std::cerr << "Lua Error: " << error << std::endl;
        lua_pop(L, 1);
        lua_close(L);
        return 1;
    }*/

    bool mainScriptFinished = (mainStatus == LUA_OK);
    callLuaCallback(L, "ready");

    double lastTime = getCurrentTime();
    double physicsAccumulator = 0.0;

    // wait for the window before executing physics_step/draw
    // idk if this is the best thing, but why would you want physics_step/draw without a window
    while (!g_window && !g_window->isRunning){
        std::cout << "waiting for window" << std::endl;
        sleep(100);
    }

    double fpsTimer = 0;
    int frameCount = 0;
    int currentFPS = 0;

    while (g_window && g_window->isRunning){
        double currentTime = getCurrentTime();
        double dt = currentTime-lastTime;
        lastTime = currentTime;

        if (g_mouse) g_mouse->update();
        if (g_keyboard) g_keyboard->update();
        
        pullEventsWithInput();

        CURRENT_TICK++;

        // calculate fps thing
        frameCount++;
        fpsTimer += dt;
        if (fpsTimer >= 1.0) {
            currentFPS = frameCount;
            g_window->currentFPS = currentFPS;
            frameCount = 0;
            fpsTimer -= 1.0;
        }

        if (g_window != nullptr){
            PHYSICS_DT = 1.0/g_window->get_physics_framerate();
        }

        physicsAccumulator += dt;
        while (physicsAccumulator >= PHYSICS_DT){
            callLuaCallback(L, "physics_step", PHYSICS_DT);
            physicsAccumulator -= PHYSICS_DT;
            if (!mainScriptFinished && mainStatus == LUA_YIELD){
                mainStatus = lua_resume(mainThread, 0);
                if (mainStatus != LUA_OK && mainStatus != LUA_YIELD){
                    std::cerr << "Main script error: " << lua_tostring(mainThread, -1) << std::endl;
                    mainScriptFinished = true;
                } else if (mainStatus == LUA_OK){
                    mainScriptFinished = true;
                    std::cout << "Main script finished execution" << std::endl;
                }
            }
        }

        g_window->clear(0, 0, 0);
        callLuaCallback(L, "draw");
        g_renderer->flush();
        g_window->present();

        if (g_window->getTargetFPS() > 0){
            double targetFrameTime = 1.0/g_window->getTargetFPS();
            double frameTime = getCurrentTime() - currentTime;
            if (frameTime < targetFrameTime){
                uint32_t delayms = static_cast<uint32_t>((targetFrameTime - frameTime)*1000);
                if (delayms > 0){
                    SDL_Delay(delayms);
                }
            }
        }
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