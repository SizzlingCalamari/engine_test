
#pragma once

#include <SDL_audio.h>

class AudioDeviceImpl;

namespace SDLAudio
{
    struct AudioDeviceDesc
    {
        AudioDeviceDesc() = default;

        const char* deviceName;
        SDL_AudioSpec audioSpec;
    };

    class AudioDevice
    {
    public:
        using Impl = AudioDeviceImpl;

        const AudioDeviceDesc& GetDeviceDesc() const;

        void StartPlayback();
        void PushAudio(const void* data, int numBytes);

    protected:
        AudioDevice() = default;
        ~AudioDevice() = default;
        AudioDevice(const AudioDevice&) = delete;
        AudioDevice(const AudioDevice&&) = delete;
        AudioDevice& operator=(const AudioDevice&) = delete;
        AudioDevice& operator=(const AudioDevice&&) = delete;
    };
}
