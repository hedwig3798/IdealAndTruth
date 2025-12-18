#pragma once
#include <vector>

#include "Vertex.h"

void CreateSphere(
	OUT std::vector<VertexPU>& _vertex
	, OUT std::vector<UINT>& _index
	, bool _isFrontCCW = true
);