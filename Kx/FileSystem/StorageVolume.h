#pragma once
#include "Common.h"
#include "Kx/General/UUID.h"
#include "KxFramework/KxWinUndef.h"

namespace KxFramework
{
	class LegacyVolume;
}

namespace KxFramework
{
	class KX_API StorageVolume final
	{
		public:
			static size_t Enumerate(std::function<bool(StorageVolume)> func);

		private:
			UUID m_GUID;

		public:
			StorageVolume() = default;
			StorageVolume(StorageVolume&&) = default;
			StorageVolume(const StorageVolume&) = default;

		public:
			bool IsValid() const;
			bool DoesExist() const;

		public:
			explicit operator bool() const noexcept
			{
				return IsValid();
			}
			bool operator!() const noexcept
			{
				return !IsValid();
			}
	
			StorageVolume& operator=(StorageVolume&&) = default;
			StorageVolume& operator=(const StorageVolume&) = default;
	};
}
