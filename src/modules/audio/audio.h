#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

struct Sound{
    Mix_Chunk* chunk;
    Mix_Chunk* originalChunk;
    int channel;
    float volume;
    float pitch;
    bool looping;
};

class Audio{
public:
    Audio();
    ~Audio();

    bool init();

    int load(const std::string& path);
    void unload(int id);

    void play(int id, bool loop = false);
    void stop(int id);
    void pause(int id);
    void resume(int id);

    void setVolume(int id, float volume);
    float getVolume(int id);
    void setPitch(int id, float pitch);
    float getPitch(int id);
    void setLooping(int id, bool loop);
    bool isLooping(int id);

    bool isPlaying(int id);
    bool isPaused(int id);

    void setMasterVolume(float volume);
    float getMasterVolume();
    void stopAll();
    void pauseAll();
    void resumeAll();
private:
    std::unordered_map<int, Sound> sounds;
    Mix_Chunk* resampleChunk(Mix_Chunk* original, float pitch);
    int nextId;
    float masterVolume;
};
