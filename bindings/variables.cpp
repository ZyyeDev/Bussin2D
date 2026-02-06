#include "variables.h"

Renderer* g_renderer = nullptr;
Window* g_window = nullptr;
Audio* g_audio = nullptr;
Keyboard* g_keyboard = nullptr;
Mouse* g_mouse = nullptr;
Camera* g_camera = nullptr;
VFS* g_vfs = nullptr;

int CURRENT_TICK = 0;

void init_camera(){
    if (!g_camera && g_window){
        g_camera = new Camera(g_window->getWidth(), g_window->getHeight());
    }
}

void init_renderer(){
    if (g_window && !g_renderer){
        g_renderer = new Renderer(g_window->getWidth(), g_window->getHeight());
    }
}

void init_vfs(){
    if (!g_vfs){
        g_vfs = &VFS::get();
    }
}