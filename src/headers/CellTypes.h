#pragma once

namespace CellTypes {
	const float J[7][7] = {
		{1000000.0f, 1000000.0f,	1000000.0f, 1000000.0f,	1000000.0f,	1000000.0f, 1000000.0f},
		{1000000.0f, 0.0f,			0.0f,		50.0f,		50.0f,		50.0f,		50.0f},
		{1000000.0f, 0.0f,			0.0f,		50.0f,		50.0f,		50.0f,		50.0f},
		{1000000.0f, 0.0f,			0.0f,		35.0f,		40.0f,		100.0f,		100.0f},
		{1000000.0f, 0.0f,			0.0f,		40.0f,		20.0f,		100.0f,		100.0f},
		{1000000.0f, 0.0f,			0.0f,		0.0f,		0.0f,		15.0f,		50.0f},
		{1000000.0f, 0.0f,			0.0f,		0.0f,		0.0f,		50.0f,		20.0f}
	};
}

enum class CELL_TYPE {
	BOUNDARY = 0,
	EMPTYSPACE = 1,
	FLUID = 2,
	GENERIC = 3,
	GENERIC_COMPACT = 4,
	TROPHECTODERM = 5,
	ICM = 6
};
