
#pragma once

#include "../public/hAudioSystem.h"
#include "hAudioDeviceImpl.h"
#include <vector>

class AudioSystemImpl: public SDLAudio::AudioSystem
{
public:
    AudioSystemImpl();

    StrList     GetAudioDrivers() const;
    const char* GetCurrentAudioDriver() const;

    void    RefreshDevices();
    StrList GetAudioDevices() const;
    auto    OpenAudioDevice(SDLAudio::AudioDeviceDesc& desc) -> SDLAudio::AudioDevice*;
    void    CloseAudioDevice(SDLAudio::AudioDevice*& device);

private:
    void RefreshDrivers();

private:
    std::vector<AudioDeviceImpl*> m_activeDevices;
    std::vector<const char*> m_drivers;
    std::vector<const char*> m_devices;
    const char* m_currentDriver;
};
