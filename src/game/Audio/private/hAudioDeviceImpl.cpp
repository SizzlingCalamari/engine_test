
#include "hAudioDeviceImpl.h"
#include <SDL_audio.h>

AudioDeviceImpl::AudioDeviceImpl(SDL_AudioDeviceID deviceId,
                                 const SDLAudio::AudioDeviceDesc& desc):
    SDLAudio::AudioDevice(),
    m_deviceId(deviceId),
    m_deviceDesc(desc)
{
    if (!desc.deviceName)
    {
        m_deviceDesc.deviceName = "default";
    }
}

const SDLAudio::AudioDeviceDesc& AudioDeviceImpl::GetDeviceDesc() const
{
    return m_deviceDesc;
}

void AudioDeviceImpl::StartPlayback()
{
    SDL_PauseAudioDevice(m_deviceId, 0);
}

void AudioDeviceImpl::PushAudio(const void* data, int numBytes)
{
    SDL_QueueAudio(m_deviceId, data, numBytes);
}
