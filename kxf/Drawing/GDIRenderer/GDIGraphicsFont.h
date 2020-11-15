#pragma once
#include "Common.h"
#include "../Font.h"
#include "GDIGraphicsRenderer.h"
#include "../IGDIObject.h"
#include "../GraphicsRenderer/IGraphicsFont.h"

namespace kxf
{
	class KX_API GDIGraphicsFont: public RTTI::ExtendInterface<GDIGraphicsFont, IGraphicsFont, IGDIObject>
	{
		KxRTTI_DeclareIID(GDIGraphicsFont, {0x166c83ff, 0xc5cf, 0x4098, {0x9c, 0x7c, 0xa8, 0x4b, 0x21, 0xfa, 0xe3, 0x9c}});

		protected:
			GDIGraphicsRenderer* m_Renderer = nullptr;
			Font m_Font;
			Color m_Color;

		private:
			bool DoIsSameAs(const IObject& other) const
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDIGraphicsFont>())
				{
					return m_Font.IsSameAs(object->m_Font);
				}
				return false;
			}

		public:
			GDIGraphicsFont() noexcept = default;
			GDIGraphicsFont(GDIGraphicsRenderer& rendrer)
				:m_Renderer(&rendrer)
			{
			}
			GDIGraphicsFont(GDIGraphicsRenderer& rendrer, const Font& font, const Color& color)
				:m_Renderer(&rendrer), m_Font(font), m_Color(color)
			{
			}
			GDIGraphicsFont(GDIGraphicsRenderer& rendrer, const SizeF& pixelSize, const String& faceName, const Color& color)
				:m_Renderer(&rendrer), m_Font(pixelSize, FontFamily::Default, FontStyle::Normal, FontWeight::Normal, faceName), m_Color(color)
			{
			}

		public:
			// IGDIObject
			bool IsSameAs(const IGDIObject& other) const override
			{
				return DoIsSameAs(other);
			}
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIGraphicsFont>(*this);
			}

			void* GetHandle() const override
			{
				return m_Font.GetHandle();
			}
			void* DetachHandle() override
			{
				return m_Font.DetachHandle();
			}
			void AttachHandle(void* handle) override
			{
				m_Font.AttachHandle(handle);
			}

			// IGraphicsObject
			bool IsNull() const override
			{
				return !m_Renderer || m_Font.IsNull();
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return DoIsSameAs(other);
			}
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return std::make_unique<GDIGraphicsFont>(*this);
			}

			GDIGraphicsRenderer& GetRenderer() override
			{
				return *m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return m_Font.GetHandle();
			}

			// IGraphicsFont
			Color GetColor() const override
			{
				return m_Color;
			}
			void SetColor(const Color& color) override
			{
				m_Color = color;
			}

			String GetFaceName() const override
			{
				return m_Font.GetFaceName();
			}
			void SetFaceName(const String& faceName) override
			{
				m_Font.SetFaceName(faceName);
			}

			FontFamily GetFamily() const override
			{
				return m_Font.GetFamily();
			}
			void SetFamily(FontFamily family) override
			{
				m_Font.SetFamily(family);
			}

			GraphicsFontMetrics GetMetrics() const override;
			SizeF GetPixelSize() const override
			{
				return m_Font.GetPixelSize();
			}
			void SetPixelSize(const SizeF& pixelSize) override
			{
				m_Font.SetPixelSize(pixelSize);
			}

			// GDIGraphicsFont
			const Font& Get() const
			{
				return m_Font;
			}
			Font& Get()
			{
				return m_Font;
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
