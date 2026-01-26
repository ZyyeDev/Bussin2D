#include "audio.h"
#include "core/common.h"

Audio::Audio(){
    nextId = 1;
    masterVolume = 1.0f;
}

Audio::~Audio(){
    for (auto& pair : sounds)
    {
        if (pair.second.chunk){
            Mix_FreeChunk(pair.second.chunk);
        }
    }
    sounds.clear();
    Mix_CloseAudio();
}

bool Audio::init(){
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        std::cerr << "SDL_Mixer init failed: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_AllocateChannels(32);
    return true;
}

int Audio::load(const std::string& path){
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk){
        std::cerr << "Failed to load sound " << path << " Error: " << Mix_GetError() << std::endl;
        return -1;
    }
    
    Sound sound;
    sound.chunk = chunk;
    sound.channel = -1;
    sound.pitch = 1.0f;
    sound.volume = 1.0f;
    sound.looping = false;

    int id = nextId++;
    sounds[id] = sound;
    return id;
}

void Audio::unload(int id){
    auto it = sounds.find(id);
    if (it == sounds.end()) return;

    if (it->second.channel != -1){
        Mix_HaltChannel(it->second.channel);
    }
    if (it->second.chunk){
        Mix_FreeChunk(it->second.chunk);
    }
    sounds.erase(it);
}

void Audio::play(int id, bool loop){
    auto it = sounds.find(id);
    if (it == sounds.end()) return;

    it->second.looping = loop;
    int loops;
    if (loop){
        loops = -1;
    }else{
        loops = 0;
    }
    it->second.channel = Mix_PlayChannel(-1, it->second.chunk,loops);

    if (it->second.channel != -1){
        int vol = (int)(it->second.volume * masterVolume*MIX_MAX_VOLUME);
        Mix_Volume(it->second.channel, vol);
    }
}

void Audio::stop(int id){
    auto it = sounds.find(id);
    if (it == sounds.end()) return;

    if (it->second.channel != -1){
        Mix_HaltChannel(it->second.channel);
        it->second.channel = -1;
    }
}

void Audio::pause(int id){
    auto it = sounds.find(id);
    if (it == sounds.end()) return;
    
    if (it->second.channel != -1){
        Mix_Pause(it->second.channel);
    }
}

void Audio::resume(int id){
    auto it = sounds.find(id);
    if (it == sounds.end()) return;
    
    if (it->second.channel != -1){
        Mix_Resume(it->second.channel);
    }
}

void Audio::setVolume(int id, float volume){
    auto it = sounds.find(id);
    if (it == sounds.end()) return;

    it->second.volume = volume;
    
    if (it->second.channel != -1){
        int vol = (int)(it->second.volume * masterVolume*MIX_MAX_VOLUME);
        Mix_Volume(it->second.channel, vol);
    }
}

float Audio::getVolume(int id){
    auto it = sounds.find(id);
    if (it == sounds.end()) return 0.0f;
    
    return it->second.volume;
}

void Audio::setPitch(int id, float pitch){
    auto it = sounds.find(id);
    if (it == sounds.end()) return;

    it->second.pitch = pitch;
}

float Audio::getPitch(int id){
    auto it = sounds.find(id);
    if (it == sounds.end()) return 0.0f;

    return it->second.pitch;
}

void Audio::setLooping(int id, bool loop){
    auto it = sounds.find(id);
    if (it == sounds.end()) return;

    it->second.looping = loop;
}

bool Audio::isLooping(int id){
    auto it = sounds.find(id);
    if (it == sounds.end()) return false;

    return it->second.looping;
}

bool Audio::isPlaying(int id){
    auto it = sounds.find(id);
    if (it == sounds.end()) return false;
    if (it->second.channel == -1) return false;
    return Mix_Playing(it->second.channel) != 0;
}

bool Audio::isPaused(int id){
    auto it = sounds.find(id);
    if (it == sounds.end()) return false;
    if (it->second.channel == -1) return false;
    return Mix_Paused(it->second.channel) != 0;
}

// i think this should be reworked, along with other calls to volume
void Audio::setMasterVolume(float volume){
    masterVolume = volume;
    for (auto& pair : sounds){
        if (pair.second.channel != -1){
            int vol = (int)(pair.second.volume * masterVolume * MIX_MAX_VOLUME);
            Mix_Volume(pair.second.channel, vol);
        }
    }
}

float Audio::getMasterVolume(){
    return masterVolume;
}

void Audio::stopAll(){
    Mix_HaltChannel(-1);
    for (auto& pair : sounds){
        pair.second.channel = -1;
    }
}

void Audio::pauseAll(){
    Mix_Pause(-1);
}

void Audio::resumeAll(){
    Mix_Resume(-1);
}