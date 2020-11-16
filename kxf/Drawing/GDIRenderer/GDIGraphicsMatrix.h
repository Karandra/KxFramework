#pragma once
#include "Common.h"
#include "GDIGraphicsRenderer.h"
#include "IGDIObject.h"
#include "../GraphicsRenderer/IGraphicsMatrix.h"
#include "Private/Common.h"
#include <wx/affinematrix2d.h>

namespace kxf
{
	class KX_API GDIGraphicsMatrix: public RTTI::ExtendInterface<GDIGraphicsMatrix, IGraphicsMatrix>
	{
		KxRTTI_DeclareIID(GDIGraphicsMatrix, {0x4d9393ae, 0xa82e, 0x498b, {0x90, 0xa9, 0xb6, 0x7, 0xb0, 0x9f, 0x4e, 0x5f}});

		protected:
			GDIGraphicsRenderer* m_Renderer = nullptr;
			std::optional<wxAffineMatrix2D> m_Matrix;

		private:
			bool DoIsSameAs(const IObject& other) const
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDIGraphicsMatrix>())
				{
					return m_Matrix == object->m_Matrix;
				}
				return false;
			}

		public:
			GDIGraphicsMatrix() noexcept = default;
			GDIGraphicsMatrix(GDIGraphicsRenderer& rendrer, const wxAffineMatrix2D& matrix)
				:m_Renderer(&rendrer), m_Matrix(matrix)
			{
			}
			GDIGraphicsMatrix(GDIGraphicsRenderer& rendrer, float m11, float m12, float m21, float m22, float tx, float ty)
				:m_Renderer(&rendrer), m_Matrix(Drawing::Private::ToAffineMatrix2D(m11, m12, m21, m22, tx, ty))
			{
			}

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || !m_Matrix.has_value();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return DoIsSameAs(other);
			}
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<GDIGraphicsMatrix>(*this);
			}

			GDIGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return nullptr;
			}

			// IGraphicsMatrix
			bool IsIdentity() const override
			{
				return m_Matrix->IsIdentity();
			}

			void GetElements(float& m11, float& m12, float& m21, float& m22, float& tx, float& ty) const override
			{
				Drawing::Private::FromAffineMatrix2D(*m_Matrix, m11, m12, m21, m22, tx, ty);
			}
			void SetElements(float m11, float m12, float m21, float m22, float tx, float ty) override
			{
				m_Matrix = Drawing::Private::ToAffineMatrix2D(m11, m12, m21, m22, tx, ty);
			}

			bool Invert() override
			{
				return m_Matrix->Invert();
			}
			void RotateTransform(Angle angle) override
			{
				m_Matrix->Rotate(angle.ToRadians());
			}
			void Concat(const IGraphicsMatrix& other) override
			{
				if (auto affineMatrix = Drawing::Private::ToAffineMatrix2D(other))
				{
					m_Matrix->Concat(*affineMatrix);
				}
			}

			void ScaleTransform(float xScale, float yScale) override
			{
				m_Matrix->Scale(xScale, yScale);
			}
			void TranslateTransform(const Size& dxy) override
			{
				m_Matrix->Translate(dxy.GetWidth(), dxy.GetHeight());
			}
			SizeF TranslateDistance(const SizeF& dxy) override
			{
				double x = dxy.GetWidth();
				double y = dxy.GetHeight();
				m_Matrix->TransformDistance(&x, &y);
				return SizeF(x, y);
			}
			PointF TransformPoint(const PointF& xy) override
			{
				double x = xy.GetX();
				double y = xy.GetY();
				m_Matrix->TransformPoint(&x, &y);
				return PointF(x, y);
			}

			// GDIGraphicsMatrix
			const wxAffineMatrix2D& Get() const
			{
				return *m_Matrix;
			}
			wxAffineMatrix2D& Get()
			{
				return *m_Matrix;
			}

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}
	};
}
