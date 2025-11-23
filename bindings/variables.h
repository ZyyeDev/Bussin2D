#pragma once

#include "modules/graphics/renderer.h"
#include "modules/window/window.h"
#include "modules/audio/audio.h"
#include "modules/keyboard/keyboard.h"
#include "modules/mouse/mouse.h"

extern Renderer* g_renderer;
extern Window* g_window;
extern Audio* g_audio;
extern Keyboard* g_keyboard;
extern Mouse* g_mouse;