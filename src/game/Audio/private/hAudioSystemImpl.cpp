
#include "hAudioSystemImpl.h"
#include <SDL_audio.h>

AudioSystemImpl::AudioSystemImpl():
    SDLAudio::AudioSystem(),
    m_currentDriver(SDL_GetCurrentAudioDriver())
{
    RefreshDrivers();
    RefreshDevices();
}

IteratorPair<const char* const*> AudioSystemImpl::GetAudioDrivers() const
{
    auto data = m_drivers.data();
    return IteratorPairHelper(data, data + m_drivers.size());
}

const char* AudioSystemImpl::GetCurrentAudioDriver() const
{
    return m_currentDriver;
}

IteratorPair<const char* const*> AudioSystemImpl::GetAudioDevices() const
{
    auto data = m_devices.data();
    return IteratorPairHelper(data, data + m_devices.size());
}

SDLAudio::AudioDevice* AudioSystemImpl::OpenAudioDevice(SDLAudio::AudioDeviceDesc& desc)
{
    SDL_AudioSpec* audioSpec = &desc.audioSpec;
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(desc.deviceName, 0,
                                                     audioSpec, audioSpec, 0);
    if (deviceId == 0)
    {
        return nullptr;
    }

    AudioDeviceImpl* device = new AudioDeviceImpl(deviceId, desc);
    m_activeDevices.emplace_back(device);
    return device;
}

void AudioSystemImpl::RefreshDrivers()
{
    const int numDrivers = SDL_GetNumAudioDrivers();
    m_drivers.resize(numDrivers);
    const char** drivers = m_drivers.data();
    for (int i = 0; i < numDrivers; ++i)
    {
        drivers[i] = SDL_GetAudioDriver(i);
    }
}

void AudioSystemImpl::RefreshDevices()
{
    const int numDevices = SDL_GetNumAudioDevices(0);
    m_devices.resize(numDevices);
    const char** devices = m_devices.data();
    for (int i = 0; i < numDevices; ++i)
    {
        devices[i] = SDL_GetAudioDeviceName(i, 0);
    }
}
