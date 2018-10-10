#pragma once

#include "UI.h"

class NumUI_Inst : public UI
{
	struct tagNumUI
	{
		InstanceData		instData;
		float				fStayTime = 0.f;
		bool				isCritical;
	};

public:
	NumUI_Inst(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize);
	~NumUI_Inst();

public:
	HRESULT					Initialize();
	virtual bool			Update(const GameTimer & gt);
	virtual void			Render(ID3D12GraphicsCommandList* cmdList);

	void					Add(int iNum, XMFLOAT3 f3Pos, bool isCritical = false);

public:
	static NumUI_Inst* Create(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> &srv, UINT srvSize);

private:
	vector<tagNumUI>			m_vNum;

	unsigned long				m_iMyInstObject;
	std::vector<InstanceData>	vecInstances;

	XMFLOAT4X4					m_f4x4InitWorld;

private:
	virtual void Free();
};

