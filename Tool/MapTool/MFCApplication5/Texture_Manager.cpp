#include "stdafx.h"
#include "Texture_Manager.h"

IMPLEMENT_SINGLETON(CTexture_Manager);

CTexture_Manager::CTexture_Manager()
{

}

CTexture_Manager::~CTexture_Manager()
{
}

void CTexture_Manager::Ready_Texture(wstring TexName, Texture* tex, HEAP_TYPE eType)
{
	if (Find_Texture(TexName, eType) != nullptr)
		return;

	m_mapTexture[eType].emplace(TexName, tex);
}

Texture* CTexture_Manager::Find_Texture(wstring TexName, HEAP_TYPE eType)
{
	auto& iter = m_mapTexture[eType].find(TexName);

	if (iter == m_mapTexture[eType].end())
		return nullptr;

	return iter->second;
}

void CTexture_Manager::Free()
{
}
