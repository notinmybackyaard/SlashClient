#pragma once

template <typename T>
void Safe_Delete(T& t)
{
	if (NULL != t)
	{
		delete t;
		t = NULL;
	}
}

template <typename T>
void Safe_Delete_Array(T& t)
{
	if (NULL != t)
	{
		delete[] t;
		t = NULL;
	}
}

template <typename T>
unsigned long Safe_Release(T& t)
{
	unsigned long dwRefCnt = 0;
	if (NULL != t)
	{
		dwRefCnt = t->Release();

		if (0 == dwRefCnt)
			t = NULL;
	}

	return dwRefCnt;
}

//넘겨준 벡터의 idx번째 원소를 마지막 원소와 바꾼후 erase()한다.
template<typename T>
void Erase_Vector_Element(vector<T>& _vec, size_t _idx)
{
	if (_idx >= _vec.size()) return;
	std::swap(_vec[_idx], _vec.back());
	_vec.erase((--_vec.end()));
}


// XMFLOAT3

static DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& f3Src, const float fDev)
{
	return DirectX::XMFLOAT3(f3Src.x / fDev, f3Src.y / fDev, f3Src.z / fDev);
}

static void operator+=(DirectX::XMFLOAT3& f3Src, const DirectX::XMFLOAT3& f3Desc)
{
	f3Src.x += f3Desc.x;
	f3Src.y += f3Desc.y;
	f3Src.z += f3Desc.z;
}

static void operator+=(DirectX::XMFLOAT3& f3Src, const DirectX::XMVECTOR& vDesc)
{
	DirectX::XMFLOAT3 f3Desc;
	XMStoreFloat3(&f3Desc, vDesc);
	f3Src.x += f3Desc.x;
	f3Src.y += f3Desc.y;
	f3Src.z += f3Desc.z;
}

static DirectX::XMFLOAT3& XMFloat3Normalize(DirectX::XMFLOAT3& f3Src)
{
	float fDevide = sqrt(pow(f3Src.x, 2) + pow(f3Src.y, 2) + pow(f3Src.z, 2));
	f3Src.x /= fDevide;
	f3Src.y /= fDevide;
	f3Src.z /= fDevide;

	return f3Src;
}

// XMFLOAT4
static void operator+=(DirectX::XMFLOAT4& f4Src, const DirectX::XMFLOAT4& f4Desc)
{
	f4Src.x += f4Desc.x;
	f4Src.y += f4Desc.y;
	f4Src.z += f4Desc.z;
	f4Src.w += f4Desc.w;
}

static void operator+=(DirectX::XMFLOAT4& f4Src, const DirectX::XMVECTOR& vDesc)
{
	DirectX::XMFLOAT4 f4Desc;
	XMStoreFloat4(&f4Desc, vDesc);
	f4Src.x += f4Desc.x;
	f4Src.y += f4Desc.y;
	f4Src.z += f4Desc.z;
	f4Src.w += f4Desc.w;
}

// XMFLOAT4X4
static void operator*=(DirectX::XMFLOAT4X4& f4x4Src, const DirectX::XMFLOAT4X4& f4x4Desc)
{
	DirectX::XMMATRIX matSrc = XMLoadFloat4x4(&f4x4Src);
	DirectX::XMMATRIX matDesc = XMLoadFloat4x4(&f4x4Desc);

	matSrc = XMMatrixMultiply(matSrc, matDesc);

	XMStoreFloat4x4(&f4x4Src, matSrc);
}

static void operator*=(DirectX::XMFLOAT4X4& f4x4Src, const DirectX::XMMATRIX& matDesc)
{
	DirectX::XMMATRIX matSrc = XMLoadFloat4x4(&f4x4Src);
	matSrc = XMMatrixMultiply(matSrc, matDesc);

	XMStoreFloat4x4(&f4x4Src, matSrc);
}