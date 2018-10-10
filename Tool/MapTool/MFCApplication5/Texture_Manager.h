#pragma once

#include "Base.h"
#include "Define.h"

class CTexture_Manager
	: public CBase
{
	DECLARE_SINGLETON(CTexture_Manager)

private:
	explicit CTexture_Manager();
	virtual ~CTexture_Manager();

public:
	void		Ready_Texture(wstring TexName, Texture* tex, HEAP_TYPE eType);
	Texture*	Find_Texture(wstring TexName, HEAP_TYPE eType);
	const unordered_map<wstring, Texture*>&	Get_TextureMap(HEAP_TYPE eType) { return m_mapTexture[eType]; }

private:
	unordered_map<wstring, Texture*>			m_mapTexture[HEAP_END];

private:
	virtual void Free();
};