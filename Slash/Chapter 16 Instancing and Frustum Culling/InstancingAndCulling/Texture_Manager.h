#pragma once

#include "Base.h"
#include "Define.h"

class CTexture_Manager
	: public CBase
{
	DECLARE_SINGLETON(CTexture_Manager)
public:
	enum TEXTYPE {TEX_DEFAULT_2D, TEX_DEFAULT_CUBE, TEX_DEFAULT_BILLBOARD, TEX_INST_2D, TEX_END};

private:
	explicit CTexture_Manager();
	virtual ~CTexture_Manager();

public:
	HRESULT		Ready_Texture(string TexName, Texture* tex, TEXTYPE eType);
	Texture*	Find_Texture(string TexName, TEXTYPE eType);
	const unordered_map<string, Texture*>&	Get_TextureMap(TEXTYPE eType) { return m_mapTexture[eType]; }

private:
	unordered_map<string, Texture*>			m_mapTexture[TEX_END];

private:
	virtual void Free();
};