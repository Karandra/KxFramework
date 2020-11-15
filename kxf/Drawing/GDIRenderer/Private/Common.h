#pragma once
#include "../Common.h"
class wxAffineMatrix2D;

namespace kxf
{
	class IGraphicsMatrix;
}

namespace kxf::Drawing::Private
{
	void FromAffineMatrix2D(const wxAffineMatrix2D& affineMatrix, float& m11, float& m12, float& m21, float& m22, float& tx, float& ty) noexcept;
	std::optional<wxAffineMatrix2D> ToAffineMatrix2D(const IGraphicsMatrix& graphicsmatrix) noexcept;
	wxAffineMatrix2D ToAffineMatrix2D(float m11, float m12, float m21, float m22, float tx, float ty) noexcept;
}
