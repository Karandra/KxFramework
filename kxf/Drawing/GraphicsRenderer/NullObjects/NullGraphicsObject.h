#pragma once
#include "../Common.h"
#include "../IGraphicsObject.h"
#include "NullGraphicsRenderer.h"

namespace kxf::Drawing
{
	class KX_API NullGraphicsObject final: public IGraphicsObject
	{
		private:
			NullGraphicsRenderer m_Renderer;

		public:
			NullGraphicsObject() noexcept = default;

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
			void* GetNativeHandle() const override
			{
				return nullptr;
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsObject NullGraphicsObject;
}
