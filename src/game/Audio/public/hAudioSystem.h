
#pragma once

#include "utils.h"

class AudioSystemImpl;

namespace SDLAudio
{
    struct AudioDeviceDesc;
    class AudioDevice;

    class AudioSystem
    {
    public:
        using Impl = AudioSystemImpl;
        using StrList = IteratorPair<const char* const*>;

        StrList         GetAudioDrivers() const;
        const char*     GetCurrentAudioDriver() const;

        void            RefreshDevices();
        StrList         GetAudioDevices() const;
        AudioDevice*    OpenAudioDevice(AudioDeviceDesc& desc);
        void            CloseAudioDevice(AudioDevice*& device);

    protected:
        AudioSystem() = default;
        ~AudioSystem() = default;
        AudioSystem(const AudioSystem&) = delete;
        AudioSystem(const AudioSystem&&) = delete;
        AudioSystem& operator=(const AudioSystem&) = delete;
        AudioSystem& operator=(const AudioSystem&&) = delete;
    };

    AudioSystem* CreateAudioSystem();
    void FreeAudioSystem(AudioSystem*& sys);
}
