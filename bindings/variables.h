#pragma once

#include "modules/graphics/renderer.h"
#include "modules/window/window.h"
#include "modules/audio/audio.h"
#include "modules/keyboard/keyboard.h"
#include "modules/mouse/mouse.h"

#ifndef LUA_RENDERER
    #define LUA_RENDERER

    static Renderer* g_renderer = nullptr;
#endif
#ifndef LUA_WINDOW
    #define LUA_WINDOW

    static Window* g_window = nullptr;
#endif
#ifndef LUA_AUDIO
    #define LUA_AUDIO

    static Audio* g_audio = nullptr;
#endif
#ifndef LUA_KEYBOARD
    #define LUA_KEYBOARD

    static Keyboard* g_keyboard = nullptr;
#endif
#ifndef LUA_MOUSE
    #define LUA_MOUSE

    static Mouse* g_mouse = nullptr;
#endif
