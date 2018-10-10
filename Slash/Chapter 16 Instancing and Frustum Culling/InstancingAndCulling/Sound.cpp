#include "stdafx.h"
#include "Sound.h"


CSound::CSound(void)
{
}

CSound::~CSound(void)
{
}

HRESULT CSound::Ready_Sound(void)
{
	m_pSystem = NULL;
	ZeroMemory(m_pEffect, sizeof(void*) * maxChannel);
	m_pBGM = NULL;

	m_iVersion = 0;

	m_Result = FMOD_System_Create(&m_pSystem);
	if (FAILED(ErrorCheck(m_Result)))
		return E_FAIL;

	m_Result = FMOD_System_GetVersion(m_pSystem, &m_iVersion);
	if (FAILED(ErrorCheck(m_Result)))
		return E_FAIL;

	m_Result = FMOD_System_Init(m_pSystem, maxChannel, FMOD_INIT_NORMAL, NULL);
	if (FAILED(ErrorCheck(m_Result)))
		return E_FAIL;

	return S_OK;
}

int CSound::PlayEffect(const wchar_t* pCategory, const wchar_t * pTag, float _volume)
{
	auto& category = m_mapSound.find(pCategory);
	if (category == m_mapSound.end())
		return -1;

	auto& effect = category->second.find(pTag);
	if (effect == category->second.end()) 
		return -1;
	//재생중이 아닌 채널을 찾아 재생
	for (int i = 0; i < maxChannel; ++i) {
		FMOD_BOOL isPlay = false;

		FMOD_Channel_IsPlaying(m_pEffect[i], &isPlay);
		if (!isPlay) {
			FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, effect->second, false, &m_pEffect[i]);
			FMOD_Channel_SetVolume(m_pEffect[i], _volume);
			return i;
		}
	}
	return -1;
}

void CSound::PlayBGM(const wchar_t* pCategory, const wchar_t * pTag, float _volume)
{
	auto& category = m_mapSound.find(pCategory);
	if (category == m_mapSound.end())
		return;

	auto& effect = category->second.find(pTag);
	if (effect == category->second.end())
		return;

	FMOD_Sound_SetMode(effect->second, FMOD_LOOP_NORMAL);

	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, effect->second, false, &m_pBGM);
	FMOD_Channel_SetVolume(m_pBGM, _volume);
}

bool CSound::IsPlaying(int _iChannel)
{
	FMOD_BOOL IsPlaying = 0;
	if (m_pEffect[_iChannel] != NULL)
	{
		FMOD_Channel_IsPlaying(m_pEffect[_iChannel], &IsPlaying);
	}
	return IsPlaying > 0;
}

bool CSound::IsPlaying_BGM()
{
	FMOD_BOOL IsPlaying = 0;

	FMOD_Channel_IsPlaying(m_pBGM, &IsPlaying);
	
	return IsPlaying > 0;
}

void CSound::StopAll(void)
{
	for (int i = 0; i < maxChannel; ++i)
		m_Result = FMOD_Channel_Stop(m_pEffect[i]);
	m_Result = FMOD_Channel_Stop(m_pBGM);
}

void CSound::StopBGM(void)
{
	m_Result = FMOD_Channel_Stop(m_pBGM);
}

void CSound::StopEffect(void)
{
	for (int i = 0; i < maxChannel; ++i)
		m_Result = FMOD_Channel_Stop(m_pEffect[i]);
}

void CSound::StopEffect(int _iChannel)
{
	m_Result = FMOD_Channel_Stop(m_pEffect[_iChannel]);
}

HRESULT CSound::SoundFileLoadFromPath(fs::path pFilePath)
{
	if (!fs::is_directory(pFilePath)) {
		MSG_BOX((pFilePath.wstring() + L" does not exist or not directory").c_str());
		return E_FAIL;
	}

	for (const auto& directory : fs::directory_iterator{ pFilePath }) {
		if (fs::is_regular_file(directory.status())) {
			fs::path filePath = directory.path();

			FMOD_SOUND*	pSound = nullptr;

			m_Result = FMOD_System_CreateSound(m_pSystem, converter.to_bytes(filePath).c_str(), FMOD_HARDWARE, 0, &pSound);

			auto& category = m_mapSound.find(pFilePath.stem());
			if (category == m_mapSound.end())
				m_mapSound.emplace(pFilePath.stem(), unordered_map<wstring, FMOD_SOUND*>());

			m_mapSound[pFilePath.stem()].emplace(filePath.stem(), pSound);
		}
	}

	return S_OK;
}

HRESULT CSound::Load_Sound(fs::path pFilePath)
{
	return E_NOTIMPL;
}

void CSound::Set_Effect_Volume(int _iChannel, float _volume)
{
	FMOD_Channel_SetVolume(m_pEffect[_iChannel], _volume);
}

void CSound::Set_BGM_Volume(float _volume)
{
	FMOD_Channel_SetVolume(m_pBGM, _volume);
}

float CSound::Get_BGM_Volume()
{
	float fVolume = 0.f;
	FMOD_Channel_GetVolume(m_pBGM, &fVolume);
	return fVolume;
}

HRESULT CSound::ErrorCheck(FMOD_RESULT _result)
{
	if (_result != FMOD_OK)
	{
		MSG_BOX((wstring(L"Sound Error - ") + converter.from_bytes(FMOD_ErrorString(_result))).c_str());
		return E_FAIL;
	}
	return S_OK;
}

CSound * CSound::Create(void)
{
	auto pInstance = new CSound();
	if (FAILED(pInstance->Ready_Sound()))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent* CSound::Clone(void)
{
	AddRef();
	return this;
}

void CSound::Free(void)
{
	for (auto& soundMap : m_mapSound) {
		for (auto& sound : soundMap.second)
		{
			m_Result = FMOD_Sound_Release(sound.second);
			ErrorCheck(m_Result);
		}
		soundMap.second.clear();
	}
	m_mapSound.clear();

	m_Result = FMOD_System_Close(m_pSystem);
	ErrorCheck(m_Result);

	m_Result = FMOD_System_Release(m_pSystem);
	ErrorCheck(m_Result);
}
