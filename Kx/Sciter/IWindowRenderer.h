#pragma once
#include "Common.h"

namespace KxSciter
{
	class Host;
}

namespace KxSciter
{
	class KX_API IWindowRenderer
	{
		public:
			static std::unique_ptr<IWindowRenderer> CreateInstance(WindowRenderer type, Host& host);

		public:
			IWindowRenderer() = default;
			virtual ~IWindowRenderer() = default;

		public:
			virtual bool Create() = 0;
			virtual void Render() = 0;
			virtual void OnSize() = 0;
			virtual void OnIdle() = 0;

			virtual double GetFPS() const = 0;
	};
}
