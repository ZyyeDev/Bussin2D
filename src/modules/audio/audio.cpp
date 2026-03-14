#include "audio.h"
#include "core/common.h"
#include "modules/vfs/vfs.h"

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
    Mix_Quit();
}

bool Audio::init(){
    int flags = MIX_INIT_OGG;
    if ((Mix_Init(flags) & flags) != flags) {
        std::cerr << "SDL_Mixer Init OGG failed: " << Mix_GetError() << std::endl;
        return false;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        std::cerr << "SDL_Mixer init failed: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_AllocateChannels(32);
    return true;
}

int Audio::load(const std::string& path){
    Mix_Chunk* chunk;

    VFS vfs = VFS::get();

    if (vfs.isPackaged()){
        auto data = vfs.readBinary(path);

        if (!data.empty()){
            SDL_RWops* rwops = SDL_RWFromConstMem(data.data(), data.size());

            if (rwops){
                chunk = Mix_LoadWAV_RW(rwops, 1);
            }
        }
    }else{
        chunk = Mix_LoadWAV(vfs.resolvePath(path).c_str());
    }

    if (!chunk){
        std::cerr << "Failed to load sound " << path << " Error: " << Mix_GetError() << std::endl;
        return -1;
    }
    
    Sound sound;
    sound.chunk = chunk;
    sound.originalChunk = chunk;
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
    if (it->second.chunk != it->second.originalChunk){
        SDL_free(it->second.chunk->abuf);
        SDL_free(it->second.chunk);
    }
    if (it->second.originalChunk){
        Mix_FreeChunk(it->second.originalChunk);
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

    if (it->second.chunk != it->second.originalChunk){
        SDL_free(it->second.chunk->abuf);
        SDL_free(it->second.chunk);
    }

    it->second.chunk = resampleChunk(it->second.originalChunk, pitch);
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

Mix_Chunk* Audio::resampleChunk(Mix_Chunk* original, float pitch){
    if (pitch == 1.0f) return original;

    int originalSamples = original->alen / 4;
    int newSamples = (int)(originalSamples / pitch);

    Uint8* newBuffer = (Uint8*)SDL_malloc(newSamples * 4);
    Sint16* src = (Sint16*)original->abuf;
    Sint16* dst = (Sint16*)newBuffer;

    for (int i = 0; i < newSamples; i++){
        float srcIndex = i * pitch;
        int index = (int)srcIndex;
        float frac = srcIndex - index;

        // i hate this so much
        if (index >= originalSamples - 1){
            dst[i * 2] = src[(originalSamples - 1) * 2];
            dst[i * 2 + 1] = src[(originalSamples - 1) * 2 + 1];
        } else {
            dst[i * 2] = (Sint16)(src[index * 2] * (1.0f - frac) + src[(index + 1) * 2] * frac);
            dst[i * 2 + 1] = (Sint16)(src[index * 2 + 1] * (1.0f - frac) + src[(index + 1) * 2 + 1] * frac);
        }
    }

    Mix_Chunk* newChunk = (Mix_Chunk*)SDL_malloc(sizeof(Mix_Chunk));
    newChunk->allocated = 1;
    newChunk->abuf = newBuffer;
    newChunk->alen = newSamples * 4;
    newChunk->volume = original->volume;
    return newChunk;
}