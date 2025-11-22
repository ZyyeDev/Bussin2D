#ifdef _WIN32
    #include <windows.h>
    #define sleep(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define sleep(ms) usleep(ms)
#endif

#include <bindings.h>
#include "lua_window.h"
#include "lua_graphics.h"
#include "lua_audio.h"
#include "lua_keyboard.h"
#include "lua_mouse.h"

static int lua_sleep(lua_State *L){
    float m = static_cast<float> (luaL_checknumber(L,1));
    sleep(m * 1000);

    return 0;
}

void register_all_bindings(lua_State* L){
    lua_pushcfunction(L, lua_sleep);
    lua_setglobal(L, "wait");
    
    register_window_bindings(L);
    register_graphics_bindings(L);
    register_audio_bindings(L);
    register_keyboard_bindings(L);
    register_mouse_bindings(L);
};