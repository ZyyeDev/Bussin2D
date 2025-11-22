#pragma once

#include <lua.hpp>
#include "modules/audio/audio.h"
#include "variables.h"

#define SOUND_METATABLE "buss.sound"

void init_audio(){
    if (!g_audio){
        g_audio = new Audio();
        g_audio->init();
    }
}

static int lua_sound_play(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    bool loop = lua_toboolean(L, 2);
    g_audio->play(*id, loop);
    return 0;
}

static int lua_sound_stop(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    g_audio->stop(*id);
    return 0;
}

static int lua_sound_pause(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    g_audio->pause(*id);
    return 0;
}

static int lua_sound_resume(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    g_audio->resume(*id);
    return 0;
}

static int lua_sound_setVolume(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    float volume = luaL_checknumber(L, 2);
    g_audio->setVolume(*id, volume);
    return 0;
}

static int lua_sound_getVolume(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    lua_pushnumber(L, g_audio->getVolume(*id));
    return 1;
}

static int lua_sound_setPitch(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    float pitch = luaL_checknumber(L, 2);
    g_audio->setPitch(*id, pitch);
    return 0;
}

static int lua_sound_getPitch(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    lua_pushnumber(L, g_audio->getPitch(*id));
    return 1;
}

static int lua_sound_setLooping(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    bool loop = lua_toboolean(L, 2);
    g_audio->setLooping(*id, loop);
    return 1;
}

static int lua_sound_isLooping(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    lua_pushboolean(L, g_audio->isLooping(*id));
    return 1;
}

static int lua_sound_isPlaying(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    lua_pushboolean(L, g_audio->isPlaying(*id));
    return 1;
}

static int lua_sound_isPaused(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    lua_pushboolean(L, g_audio->isPaused(*id));
    return 1;
}

static int lua_sound_gc(lua_State* L){
    int* id = (int*)luaL_checkudata(L, 1, SOUND_METATABLE);
    g_audio->unload(*id);
    return 0;
}

static int lua_audio_load(lua_State* L){
    init_audio();
    const char* path = luaL_checkstring(L, 1);

    int soundId = g_audio->load(path);
    if (soundId == -1){
        lua_pushnil(L);
        return 1;
    }

    int* ud = (int*)lua_newuserdata(L, sizeof(int));
    *ud = soundId;

    luaL_getmetatable(L, SOUND_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

static int lua_audio_setMasterVolume(lua_State* L){
    init_audio();
    float vol = (float)luaL_checknumber(L,1);
    g_audio->setMasterVolume(vol);
    return 0;
}

static int lua_audio_getMasterVolume(lua_State* L){
    init_audio();
    lua_pushnumber(L, g_audio->getMasterVolume());
    return 0;
}

static int lua_audio_stopAll(lua_State* L){
    init_audio();
    g_audio->stopAll();
    return 0;
}

static int lua_audio_pauseAll(lua_State* L){
    init_audio();
    g_audio->pauseAll();
    return 0;
}

static int lua_audio_resumeAll(lua_State* L){
    init_audio();
    g_audio->resumeAll();
    return 0;
}

void register_audio_bindings(lua_State* L){
    luaL_newmetatable(L, SOUND_METATABLE);

    lua_newtable(L);
    lua_pushcfunction(L, lua_sound_play);
    lua_setfield(L, -2, "play");

    lua_pushcfunction(L, lua_sound_stop);
    lua_setfield(L, -2, "stop");

    lua_pushcfunction(L, lua_sound_pause);
    lua_setfield(L, -2, "pause");

    lua_pushcfunction(L, lua_sound_resume);
    lua_setfield(L, -2, "resume");

    lua_pushcfunction(L, lua_sound_setVolume);
    lua_setfield(L, -2, "setVolume");

    lua_pushcfunction(L, lua_sound_getVolume);
    lua_setfield(L, -2, "getVolume");

    lua_pushcfunction(L, lua_sound_setPitch);
    lua_setfield(L, -2, "setPitch");

    lua_pushcfunction(L, lua_sound_getPitch);
    lua_setfield(L, -2, "getPitch");

    lua_pushcfunction(L, lua_sound_setLooping);
    lua_setfield(L, -2, "setLooping");

    lua_pushcfunction(L, lua_sound_isLooping);
    lua_setfield(L, -2, "looping");

    lua_pushcfunction(L, lua_sound_isPlaying);
    lua_setfield(L, -2, "playing");

    lua_pushcfunction(L, lua_sound_isPaused);
    lua_setfield(L, -2, "paused");
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lua_sound_gc);
    lua_setfield(L, -2, "__gc");

    lua_pop(L, 1);

    lua_getglobal(L, "buss");
    if (lua_isnil(L, -1)){
        lua_pop(L, 1);
        lua_newtable(L);
    }

    lua_newtable(L);

    lua_pushcfunction(L, lua_audio_load);
    lua_setfield(L, -2, "load");

    lua_pushcfunction(L, lua_audio_setMasterVolume);
    lua_setfield(L, -2, "setMasterVolume");

    lua_pushcfunction(L, lua_audio_getMasterVolume);
    lua_setfield(L, -2, "getMasterVolume");

    lua_pushcfunction(L, lua_audio_stopAll);
    lua_setfield(L, -2, "stopAll");

    lua_pushcfunction(L, lua_audio_pauseAll);
    lua_setfield(L, -2, "pauseAll");

    lua_pushcfunction(L, lua_audio_resumeAll);
    lua_setfield(L, -2, "resumeAll");

    lua_setfield(L, -2, "audio");
    lua_setglobal(L, "buss");
}