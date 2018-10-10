#pragma once
#include "Macro.h"
#include "Function.h"
#include "Functor.h"
#include "Struct.h"

enum HEAP_TYPE
{
	HEAP_DEFAULT,
	HEAP_INSTANCING,
	HEAP_TEXTURE_EFFECT,
	HEAP_END
};

struct EFFECT_INFO
{
	XMFLOAT3	S_Pos;
	XMFLOAT3	E_Pos;
	XMFLOAT3	S_Size;
	XMFLOAT3	E_Size;
	XMFLOAT3	S_Rot;
	XMFLOAT3	E_Rot;
	XMFLOAT4	S_Color;
	XMFLOAT4	E_Color;
	float		LifeTime;
	float		StartTime;
	string		strName;
	string		strMeshName;
	string		strTexName;
	bool		isBillboard;

	EFFECT_INFO() {}

	EFFECT_INFO(XMFLOAT3 _sPos, XMFLOAT3 _ePos, XMFLOAT3 _sSize, XMFLOAT3 _eSize, XMFLOAT3 _sRot, XMFLOAT3 _eRot, XMFLOAT4 _sColor, XMFLOAT4 _eColor, float _lifeTime, float _startTime, string _name, string _meshName, string _texName, bool _isBillboard)
		: S_Pos(_sPos), E_Pos(_ePos), S_Size(_sSize), E_Size(_eSize), S_Rot(_sRot), E_Rot(_eRot), S_Color(_sColor), E_Color(_eColor), LifeTime(_lifeTime), StartTime(_startTime), strName(_name), strMeshName(_meshName), strTexName(_texName), isBillboard(_isBillboard)
	{}
};


struct UV_FRAME_INFO
{
	XMFLOAT2		f2maxFrame = XMFLOAT2(0.f, 0.f);					// > 전체 프레임 x, y 개수
	XMFLOAT2		f2curFrame = XMFLOAT2(0.f, 0.f);					// > 현재 프레임
	XMFLOAT2		f2FrameSize = XMFLOAT2(1.f, 1.f);					// > 한 프레임 당 x, y 픽셀 크기
	float			fFrameAcc = 0.f;									// > 진행된 정도, 한프레임의 픽셀 크기보다 커지면 다음 프레임으로 넘어간다.
	float			fSpeed = 1.f;
	int				iPlayCnt = 0;
	int				iCurCnt = 0;
	bool			isEndbyCnt = true;

	UV_FRAME_INFO() {}
	UV_FRAME_INFO(XMFLOAT2 _maxFrame, XMFLOAT2 _curFrame, XMFLOAT2 _size, float _speed, int _playCnt, bool _endCbynt)
		: f2maxFrame(_maxFrame), f2curFrame(_curFrame), f2FrameSize(_size), fSpeed(_speed), fFrameAcc(0.f), iPlayCnt(_playCnt), isEndbyCnt(_endCbynt)
	{}

};