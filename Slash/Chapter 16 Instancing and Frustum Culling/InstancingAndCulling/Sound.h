#pragma once

#include "Define.h"
#include "Component.h"

constexpr	unsigned long	maxChannel = 64;

class CSound 
	: public CComponent
{
	enum SOUND_TYPE { SND_EFFECT, SND_BGM, SND_END };
protected:
	explicit	CSound(void);
	virtual		~CSound(void);

protected:
	//FMOD_SYSTEM*	m_pSystem;				//fmod의 시스템 초기화
	//FMOD_CHANNEL*	m_pEffect[maxChannel];	//효과음 채널
	//FMOD_CHANNEL*	m_pBGM;					//배경음 채널
	//FMOD_RESULT		m_Result;

	FMOD_SYSTEM*	m_pSystem;				//fmod의 시스템 초기화
	FMOD_CHANNEL*	m_pEffect[maxChannel];	//효과음 채널
	FMOD_CHANNEL*	m_pBGM;					//배경음 채널
	FMOD_RESULT		m_Result;

	unsigned int	m_iVersion;

	unordered_map<wstring,unordered_map<wstring, FMOD_SOUND*>>	m_mapSound;

public:
	HRESULT	Ready_Sound(void);
	int		PlayEffect(const wchar_t* pCategory, const wchar_t* pTag, float _volume = 1.f);
	void	PlayBGM(const wchar_t* pCategory, const wchar_t* pTag, float _volume = 1.f);
	bool	IsPlaying(int _iChannel);
	bool	IsPlaying_BGM();
	void	StopAll(void);
	void	StopBGM(void);
	void	StopEffect(void);
	void	StopEffect(int _iChannel);
	HRESULT	SoundFileLoadFromPath(fs::path pFilePath);
	HRESULT Load_Sound(fs::path pFilePath);
	void	Set_Effect_Volume(int _iChannel, float _volume);
	void	Set_BGM_Volume(float _volume);

	float	Get_BGM_Volume();

private:
	HRESULT	ErrorCheck(FMOD_RESULT _result);

public:
	static	CSound*		Create(void);
	virtual CComponent* Clone(void);

protected:
	virtual void Free(void);
};