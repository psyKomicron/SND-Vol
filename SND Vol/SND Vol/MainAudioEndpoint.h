﻿#pragma once

#include "IComEventImplementation.h"

namespace Audio
{
	class MainAudioEndpoint : public IComEventImplementation, private IAudioEndpointVolumeCallback
	{
	public:
		/**
		 * @brief Default constructor.
		 * @param devicePtr IMMDevice pointer.
		 * @param eventContextId GUID used by this app to generate events.
		*/
		MainAudioEndpoint(IMMDevice* devicePtr, GUID eventContextId);

		/**
		 * @brief Gets the volume of the session.
		 * @param desiredVolume
		 * @return True if the fonction succeeded
		*/
		float Volume() const;
		/**
		 * @brief Sets the volume of the session.
		 * @return The volume of the session
		*/
		void Volume(const float& volume);
		/**
		 * @brief Gets the friendly name of the audio endpoint.
		 * @return Friendly name of the audio endpoint
		*/
		winrt::hstring Name() const;
		/**
		 * @brief Gets the number of channels for the endpoint.
		 * @return Number of channels
		*/
		uint32_t Channels() const;
		/**
		 * @brief Gets if the endpoint is muted.
		 * @return True if the endpoint is muted
		*/
		bool Muted() const;

		inline winrt::event_token VolumeChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, float> const& handler)
		{
			return e_volumeChanged.add(handler);
		};
		void VolumeChanged(winrt::event_token const& eventToken)
		{
			e_volumeChanged.remove(eventToken);
		};
		inline winrt::event_token StateChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, bool> const& handler)
		{
			return e_stateChanged.add(handler);
		};
		inline void StateChanged(const winrt::event_token& token)
		{
			e_stateChanged.remove(token);
		};

		IFACEMETHODIMP_(ULONG) AddRef();
		IFACEMETHODIMP_(ULONG) Release();
		IFACEMETHODIMP QueryInterface(REFIID riid, VOID** ppvInterface);
		/**
		 * @brief 
		 * @return 
		*/
		bool Register();
		/**
		 * @brief 
		 * @return 
		*/
		bool Unregister();
		/**
		 * @brief Gets the current peak PCM value for the audio endpoint.
		 * @return The current peak PCM value ∈ [0, 1]
		*/
		float GetPeak() const;
		std::pair<float, float> GetPeaks();
		/**
		 * @brief Sets the audio endpoint muted or unmuted.
		 * @param mute true to mute, false to unmute
		*/
		void SetMute(const bool& mute);
		/**
		 * @brief Sets the new volume for the audio endpoint without using the "ignore notifications" GUID.
		 * @param newVolume new volume ∈ [0, 1]
		*/
		void SetVolume(const float& newVolume);

	private:
		::winrt::impl::atomic_ref_count refCount{ 1 };
		IAudioEndpointVolumePtr audioEndpointVolume{ nullptr };
		IAudioMeterInformationPtr audioMeterInfo{ nullptr };
		IMMDevicePtr device{ nullptr };
		LPWSTR deviceId = nullptr;
		GUID eventContextId;

		winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, float>> e_volumeChanged{};
		winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, bool>> e_stateChanged{};

		#pragma region IAudioEndpointVolumeCallback
		STDMETHODIMP OnNotify(__in PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
		#pragma endregion
	};
}

