#pragma once
#include "Common.h"

namespace KxFramework
{
	class KX_API IExtraData
	{
		public:
			virtual ~IExtraData() = default;
	};

	class KX_API IExtraDataContainer
	{
		public:
			enum class Type
			{
				Untyped,
				Typed
			};

		public:
			virtual ~IExtraDataContainer() = default;

		public:
			virtual Type GetType() const = 0;

			virtual void* GetExtraData() = 0;
			virtual void SetExtraData(const void* data, size_t size) = 0;
			
			virtual std::unique_ptr<IExtraData> TakeExtraObject() = 0;
			virtual IExtraData* GetExtraObject() = 0;
			virtual void SetExtraObject(std::unique_ptr<IExtraData> extraObject) = 0;
	};
}
