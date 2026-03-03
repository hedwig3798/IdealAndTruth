#pragma once

#include "VertexType.h"
#include <string>
#include <vector>

/// <summary>
/// 기본 기하 도형을 만드는 함수
/// </summary>
namespace PrimitiveGeomatry
{
	void CreateSphere(
		OUT std::vector<VertexPUNData>& _vertex
		, OUT std::vector<unsigned int>& _index
		, bool _isFrontCCW
		, unsigned int _slice = 10
		, unsigned int _stack = 10
		, float _radius = 1
	);
}