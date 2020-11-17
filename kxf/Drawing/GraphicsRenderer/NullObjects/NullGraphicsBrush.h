#pragma once
#include "../Common.h"
#include "../IGraphicsBrush.h"
#include "NullGraphicsRenderer.h"

namespace kxf::Drawing
{
	class KX_API NullGraphicsBrush final: public IGraphicsBrush
	{
		private:
			NullGraphicsRenderer m_Renderer;

		public:
			NullGraphicsBrush() noexcept = default;

		public:
			// IGraphicsObject
			bool IsNull() const override
			{
				return true;
			}
			bool IsSameAs(const IGraphicsObject& other) const override
			{
				return other.IsNull();
			}
			std::unique_ptr<IGraphicsObject> CloneGraphicsObject() const override
			{
				return nullptr;
			}

			IGraphicsRenderer& GetRenderer() override
			{
				return m_Renderer;
			}
			void* GetNativeHandle() const
			{
				return nullptr;
			}

			// IGraphicsBrush
			bool IsTransparent() const override
			{
				return true;
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsBrush NullGraphicsBrush;
}
