
#include "../public/hAudioSystem.h"
#include "hAudioCommon.h"
#include "hAudioSystemImpl.h"

namespace SDLAudio
{
    IteratorPair<const char* const*> AudioSystem::GetAudioDrivers() const
    {
        return ToImpl(this)->GetAudioDrivers();
    }

    const char* AudioSystem::GetCurrentAudioDriver() const
    {
        return ToImpl(this)->GetCurrentAudioDriver();
    }

    IteratorPair<const char* const*> AudioSystem::GetAudioDevices() const
    {
        return ToImpl(this)->GetAudioDevices();
    }

    void AudioSystem::RefreshDevices()
    {
        ToImpl(this)->RefreshDevices();
    }

    AudioDevice* AudioSystem::OpenAudioDevice(AudioDeviceDesc& desc)
    {
        return ToImpl(this)->OpenAudioDevice(desc);
    }

    AudioSystem* CreateAudioSystem()
    {
        return new AudioSystemImpl();
    }

    void FreeAudioSystem(AudioSystem*& sys)
    {
        if (sys)
        {
            delete ToImpl(sys);
            sys = nullptr;
        }
    }
}
