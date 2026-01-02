#pragma once

#ifndef BINDING_VARS

#define BINDING_VARS

#include "modules/graphics/renderer.h"
#include "modules/window/window.h"
#include "modules/audio/audio.h"
#include "modules/keyboard/keyboard.h"
#include "modules/mouse/mouse.h"
#include "modules/camera/camera.h"

extern Renderer* g_renderer;
extern Window* g_window;
extern Audio* g_audio;
extern Keyboard* g_keyboard;
extern Mouse* g_mouse;
extern Camera* g_camera;

void init_camera();
void init_renderer();

#endif