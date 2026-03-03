#include "pch.h"
#include "Geometry.h"

void PrimitiveGeomatry::CreateSphere(
	OUT std::vector<VertexPUNData>& _vertex
	, OUT std::vector<unsigned int>& _index
	, bool _isFrontCCW
	, unsigned int _slice /*= 10 */
	, unsigned int _stack /*= 10 */
	, float _radius /*= 1 */)
{
	_vertex.clear();
	_index.clear();

	// 위 꼭지점과 아래 꼭지점
	VertexPUNData topVertex;
	topVertex.m_position = { 0.0f, +_radius, 0.0f };
	topVertex.m_uv = { 0.0f, 0.0f };

	VertexPUNData bottomVertex;
	bottomVertex.m_position = { 0.0f, -_radius, 0.0f };
	bottomVertex.m_uv = { 0.0f, 1.0f };

	// 위 꼭지점 부터 시작한다.

	_vertex.push_back(topVertex);
	// y 축을 기준으로 나눴을 때 각도
	float phiStep = DirectX::XM_PI / _stack;
	// 한 줄에 만들어질 정점들 사이의 각도
	float thetaStep = 2.0f * DirectX::XM_PI / _slice;

	// 반복 시작
	for (UINT i = 1; i <= _stack - 1; ++i)
	{
		// 가로줄 각도
		float phi = i * phiStep;

		for (UINT j = 0; j <= _slice; ++j)
		{
			// 세로줄 각도
			float theta = j * thetaStep;

			VertexPUNData v;

			// 계산하기
			// 구를 위에서 보고 계산한다고 생각하면 편하다
			v.m_position.x = _radius * sinf(phi) * cosf(theta);
			v.m_position.y = _radius * cosf(phi);
			v.m_position.z = _radius * sinf(phi) * sinf(theta);

			v.m_uv.x = theta / DirectX::XM_2PI;
			v.m_uv.y = phi / DirectX::XM_PI;

			_vertex.push_back(v);
		}
	}

	// 아래 꼭지점 입력
	_vertex.push_back(bottomVertex);

	// 인덱스 넣기
	// 윗 꼭지점과 이어진 삼각형들
	// 안쪽면이 보여야 한다
	for (UINT i = 1; i <= _slice; ++i)
	{
		if (true == _isFrontCCW)
		{
			_index.push_back(0);
			_index.push_back(i);
			_index.push_back(i + 1);
		}
		else if (false == _isFrontCCW)
		{
			_index.push_back(0);
			_index.push_back(i + 1);
			_index.push_back(i);
		}
	}

	// 기본 값
	UINT baseIndex = 1;
	UINT ringVertexCount = _slice + 1;

	for (UINT i = 0; i < _stack - 2; ++i)
	{
		for (UINT j = 0; j < _slice; ++j)
		{
			if (true == _isFrontCCW)
			{
				_index.push_back(baseIndex + i * ringVertexCount + j);
				_index.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				_index.push_back(baseIndex + i * ringVertexCount + j + 1);

				_index.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				_index.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
				_index.push_back(baseIndex + i * ringVertexCount + j + 1);
			}
			else if (false == _isFrontCCW)
			{
				_index.push_back(baseIndex + i * ringVertexCount + j);
				_index.push_back(baseIndex + i * ringVertexCount + j + 1);
				_index.push_back(baseIndex + (i + 1) * ringVertexCount + j);

				_index.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				_index.push_back(baseIndex + i * ringVertexCount + j + 1);
				_index.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
			}
		}
	}

	UINT southPoleIndex = (UINT)_vertex.size() - 1;


	baseIndex = southPoleIndex - ringVertexCount;

	// 아래 꼭지점과 이어진 삼각형들
	for (UINT i = 0; i < _slice; ++i)
	{
		if (true == _isFrontCCW)
		{
			_index.push_back(southPoleIndex);
			_index.push_back(baseIndex + i + 1);
			_index.push_back(baseIndex + i);
		}
		else if (false == _isFrontCCW)
		{
			_index.push_back(southPoleIndex);
			_index.push_back(baseIndex + i);
			_index.push_back(baseIndex + i + 1);
		}
	}
}

