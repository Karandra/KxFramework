#pragma once
#include "Common.h"
#include "GDIBrush.h"
#include "GDIGraphicsRenderer.h"
#include "GDIGraphicsTexture.h"
#include "../Bitmap.h"
#include "../IGDIObject.h"
#include "../GraphicsRenderer/IGraphicsBrush.h"

namespace kxf::Drawing::Private
{
	template<class TBase>
	class KX_API GDIGraphicsBrushBase: public RTTI::ExtendInterface<GDIGraphicsBrushBase<TBase>, TBase, IGDIObject>
	{
		KxRTTI_DeclareIID(IGraphicsBrush, {0x8bc6b8e3, 0x5ae1, 0x46a9, {0x9f, 0x5a, 0xa7, 0x18, 0xcd, 0x3e, 0x5, 0x17}});

		protected:
			GDIGraphicsRenderer* m_Renderer = nullptr;
			GDIBrush m_Brush;

		private:
			bool DoIsSameAs(const IObject& other) const
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDIGraphicsBrushBase<TBase>>())
				{
					return m_Brush.IsSameAs(object->m_Brush);
				}
				return false;
			}

		public:
			GDIGraphicsBrushBase() noexcept = default;
			GDIGraphicsBrushBase(GDIGraphicsRenderer& rendrer)
				:m_Renderer(&rendrer)
			{
			}
			GDIGraphicsBrushBase(GDIGraphicsRenderer& rendrer, const GDIBrush& brush)
				:m_Renderer(&rendrer), m_Brush(brush)
			{
			}

		public:
			// IGDIObject
			bool IsSameAs(const IGDIObject& other) const override
			{
				return DoIsSameAs(other);
			}

			void* GetHandle() const override
			{
				return m_Brush.GetHandle();
			}
			void* DetachHandle() override
			{
				return m_Brush.DetachHandle();
			}
			void AttachHandle(void* handle) override
			{
				m_Brush.AttachHandle(handle);
			}

			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_Brush.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return DoIsSameAs(other);
			}

			GDIGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Brush.GetHandle();
			}

			// IGraphicsBrush
			bool IsTransparent() const override
			{
				return m_Brush.IsTransparent();
			}

			// GDIGraphicsBrush
			const GDIBrush& Get() const
			{
				return m_Brush;
			}
			GDIBrush& Get()
			{
				return m_Brush;
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

namespace kxf
{
	class KX_API GDIGraphicsSolidBrush: public Drawing::Private::GDIGraphicsBrushBase<IGraphicsSolidBrush>
	{
		public:
			GDIGraphicsSolidBrush() noexcept = default;
			GDIGraphicsSolidBrush(GDIGraphicsRenderer& rendrer, const GDIBrush& brush)
				:GDIGraphicsBrushBase(rendrer, brush)
			{
			}
			GDIGraphicsSolidBrush(GDIGraphicsRenderer& rendrer, const Color& color)
				:GDIGraphicsBrushBase(rendrer)
			{
				m_Brush.SetSolid();
				m_Brush.SetColor(color);
			}

		public:
			// IGDIObject
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIGraphicsSolidBrush>(*this);
			}

			// IGraphicsObject
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<GDIGraphicsSolidBrush>(*this);
			}

			// IGraphicsSolidBrush
			Color GetColor() const override
			{
				return m_Brush.GetColor();
			}
			void SetColor(const Color& color) override
			{
				m_Brush.SetColor(color);
			}
	};

	class KX_API GDIGraphicsHatchBrush: public Drawing::Private::GDIGraphicsBrushBase<IGraphicsHatchBrush>
	{
		private:
			Color m_ForgroundColor;

		public:
			GDIGraphicsHatchBrush() noexcept = default;
			GDIGraphicsHatchBrush(GDIGraphicsRenderer& rendrer, const GDIBrush& brush)
				:GDIGraphicsBrushBase(rendrer, brush)
			{
			}
			GDIGraphicsHatchBrush(GDIGraphicsRenderer& rendrer, HatchStyle style)
				:GDIGraphicsBrushBase(rendrer)
			{
				m_Brush.SetHatchStyle(style);
			}

		public:
			// IGDIObject
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIGraphicsHatchBrush>(*this);
			}

			// IGraphicsObject
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<GDIGraphicsHatchBrush>(*this);
			}

			// IGraphicsHatchBrush
			Color GetBackgroundColor() const override
			{
				return m_Brush.GetColor();
			}
			void SetBackgroundColor(const Color& color) override
			{
				m_Brush.SetColor(color);
			}

			Color GetForegroundColor() const override
			{
				return m_ForgroundColor;
			}
			void SetForegroundColor(const Color& color) override
			{
				m_ForgroundColor = color;
			}

			HatchStyle GetHatchStyle() const override
			{
				return m_Brush.GetHatchStyle();
			}
			void SetHatchStyle(HatchStyle style) override
			{
				m_Brush.SetHatchStyle(style);
			}
	};

	class KX_API GDIGraphicsTextureBrush: public Drawing::Private::GDIGraphicsBrushBase<IGraphicsTextureBrush>
	{
		private:
			WrapMode m_WrapMode = WrapMode::None;

		public:
			GDIGraphicsTextureBrush() noexcept = default;
			GDIGraphicsTextureBrush(GDIGraphicsRenderer& rendrer, const GDIBrush& brush, WrapMode wrapMode = WrapMode::None)
				:GDIGraphicsBrushBase(rendrer, brush), m_WrapMode(wrapMode)
			{
			}
			GDIGraphicsTextureBrush(GDIGraphicsRenderer& rendrer, const Bitmap& bitmap, WrapMode wrapMode = WrapMode::None)
				:GDIGraphicsBrushBase(rendrer), m_WrapMode(wrapMode)
			{
				m_Brush.SetStipple(bitmap);
			}
			GDIGraphicsTextureBrush(GDIGraphicsRenderer& rendrer, const Image& image, WrapMode wrapMode = WrapMode::None)
				:GDIGraphicsTextureBrush(rendrer, image.ToBitmap(), wrapMode)
			{
			}

		public:
			// IGDIObject
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIGraphicsTextureBrush>(*this);
			}

			// IGraphicsObject
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<GDIGraphicsTextureBrush>(*this);
			}

			// IGraphicsTextureBrush
			std::shared_ptr<IGraphicsTexture> GetTexture() const override
			{
				return std::make_shared<GDIGraphicsTexture>(*m_Renderer, m_Brush.GetStipple());
			}
			void SetTexture(std::shared_ptr<IGraphicsTexture> texture) override
			{
				if (texture && *texture)
				{
					m_Brush.SetStipple(texture->QueryInterface<GDIGraphicsTexture>()->Get());
				}
				else
				{
					m_Brush.SetStipple({});
				}
			}

			WrapMode GetWrapMode() const override
			{
				return m_WrapMode;
			}
			void SetWrapMode(WrapMode wrapMode) override
			{
				m_WrapMode = wrapMode;
			}
	};

	class KX_API GDIGraphicsLinearGradientBrush: public Drawing::Private::GDIGraphicsBrushBase<IGraphicsLinearGradientBrush>
	{
		private:
			RectF m_Rect;
			GradientStops m_GradentStops;
			std::shared_ptr<IGraphicsMatrix> m_Transform;
			WrapMode m_WrapMode = WrapMode::None;

		public:
			GDIGraphicsLinearGradientBrush() noexcept = default;
			GDIGraphicsLinearGradientBrush(GDIGraphicsRenderer& rendrer, const GDIBrush& brush)
				:GDIGraphicsBrushBase(rendrer, brush)
			{
			}
			GDIGraphicsLinearGradientBrush(GDIGraphicsRenderer& rendrer, const RectF& rect, const GradientStops& colors, std::shared_ptr<IGraphicsMatrix> matrix = {}, WrapMode wrapMode = WrapMode::None)
				:GDIGraphicsBrushBase(rendrer), m_Rect(rect), m_GradentStops(colors), m_Transform(std::move(matrix)), m_WrapMode(wrapMode)
			{
			}

		public:
			// IGDIObject
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIGraphicsLinearGradientBrush>(*this);
			}

			// IGraphicsObject
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<GDIGraphicsLinearGradientBrush>(*this);
			}

			// IGraphicsLinearGradientBrush
			RectF GetRect() const override
			{
				return m_Rect;
			}
			void SetRect(const RectF& rect) override
			{
				m_Rect = rect;
			}

			GradientStops GetLinearColors() const
			{
				return m_GradentStops;
			}
			void SetLinearColors(const GradientStops& colors) override
			{
				m_GradentStops = colors;
			}

			std::shared_ptr<IGraphicsMatrix> GetTransform() const override
			{
				return m_Transform;
			}
			void SetTransform(std::shared_ptr<IGraphicsMatrix> transform) override
			{
				m_Transform = std::move(transform);
			}

			WrapMode GetWrapMode() const override
			{
				return m_WrapMode;
			}
			void SetWrapMode(WrapMode wrapMode) override
			{
				m_WrapMode = wrapMode;
			}
	};

	class KX_API GDIGraphicsRadialGradientBrush: public Drawing::Private::GDIGraphicsBrushBase<IGraphicsRadialGradientBrush>
	{
		private:
			RectF m_Rect;
			GradientStops m_GradentStops;
			std::shared_ptr<IGraphicsMatrix> m_Transform;
			WrapMode m_WrapMode = WrapMode::None;

		public:
			GDIGraphicsRadialGradientBrush() noexcept = default;
			GDIGraphicsRadialGradientBrush(GDIGraphicsRenderer& rendrer, const GDIBrush& brush)
				:GDIGraphicsBrushBase(rendrer, brush)
			{
			}
			GDIGraphicsRadialGradientBrush(GDIGraphicsRenderer& rendrer, const RectF& rect, const GradientStops& colors, std::shared_ptr<IGraphicsMatrix> matrix = {}, WrapMode wrapMode = WrapMode::None)
				:GDIGraphicsBrushBase(rendrer), m_Rect(rect), m_GradentStops(colors), m_Transform(std::move(matrix)), m_WrapMode(wrapMode)
			{
			}

		public:
			// IGDIObject
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIGraphicsRadialGradientBrush>(*this);
			}

			// IGraphicsObject
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<GDIGraphicsRadialGradientBrush>(*this);
			}

			// IGraphicsRadialGradientBrush
			PointF GetCenterPoint() const override
			{
				return m_Rect.GetCenter();
			}
			void SetCenterPoint(const PointF& point) override
			{
				m_Rect.MoveCenterTo(point);
			}

			RectF GetRect() const override
			{
				return m_Rect;
			}
			void SetRect(const RectF& rect) override
			{
				m_Rect = rect;
			}

			GradientStops GetLinearColors() const
			{
				return m_GradentStops;
			}
			void SetLinearColors(const GradientStops& colors) override
			{
				m_GradentStops = colors;
			}

			std::shared_ptr<IGraphicsMatrix> GetTransform() const override
			{
				return m_Transform;
			}
			void SetTransform(std::shared_ptr<IGraphicsMatrix> transform) override
			{
				m_Transform = std::move(transform);
			}

			WrapMode GetWrapMode() const override
			{
				return m_WrapMode;
			}
			void SetWrapMode(WrapMode wrapMode) override
			{
				m_WrapMode = wrapMode;
			}
	};
}
