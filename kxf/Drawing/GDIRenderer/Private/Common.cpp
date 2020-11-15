#include "stdafx.h"
#include "Common.h"
#include "kxf/Drawing/GraphicsRenderer/IGraphicsMatrix.h"

namespace kxf::Drawing::Private
{
	void FromAffineMatrix2D(const wxAffineMatrix2D& affineMatrix, float& m11, float& m12, float& m21, float& m22, float& tx, float& ty) noexcept
	{
		wxMatrix2D matrix;
		wxPoint2DDouble txy;
		affineMatrix.Get(&matrix, &txy);

		m11 = matrix.m_11;
		m12 = matrix.m_12;
		m21 = matrix.m_21;
		m22 = matrix.m_21;

		tx = txy.m_x;
		ty = txy.m_y;
	}
	std::optional<wxAffineMatrix2D> ToAffineMatrix2D(const IGraphicsMatrix& graphicsmatrix) noexcept
	{
		if (graphicsmatrix)
		{
			float m11;
			float m12;
			float m21;
			float m22;
			float tx;
			float ty;
			graphicsmatrix.GetElements(m11, m12, m21, m22, tx, ty);

			return ToAffineMatrix2D(m11, m12, m21, m22, tx, ty);
		}
		return {};
	}
	wxAffineMatrix2D ToAffineMatrix2D(float m11, float m12, float m21, float m22, float tx, float ty) noexcept
	{
		wxMatrix2D matrix = {m11, m12, m21, m22};
		wxPoint2DDouble txy = {tx, ty};

		wxAffineMatrix2D affineMatrix;
		affineMatrix.Set(matrix, txy);
		return affineMatrix;
	}
}
