
#pragma once

#include "../public/hAudioDevice.h"
#include <SDL_audio.h>

class AudioDeviceImpl: public SDLAudio::AudioDevice
{
public:
    AudioDeviceImpl(SDL_AudioDeviceID deviceId, const SDLAudio::AudioDeviceDesc& desc);

    const SDLAudio::AudioDeviceDesc& GetDeviceDesc() const;

    void StartPlayback();
    void PushAudio(const void* data, int numBytes);

private:
    SDL_AudioDeviceID m_deviceId;
    SDLAudio::AudioDeviceDesc m_deviceDesc;
};
