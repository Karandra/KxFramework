#pragma once
#include "Common.h"
#include "kxf/Serialization/BinarySerializer.h"
#include <limits>

namespace kxf
{
	class UniversallyUniqueID;
}

namespace kxf
{
	struct NativeUUID final
	{
		friend struct std::hash<NativeUUID>;
		friend struct BinarySerializer<NativeUUID>;

		private:
			constexpr size_t GetHash() const noexcept
			{
				size_t hash = Data1;
				hash ^= Data2;
				hash ^= Data3;
				for (uint8_t d4: Data4)
				{
					hash ^= d4;
				}

				return hash;
			}

		public:
			uint32_t Data1 = 0;
			uint16_t Data2 = 0;
			uint16_t Data3 = 0;
			uint8_t Data4[8] = {};

		public:
			constexpr bool IsNull() const noexcept
			{
				return *this == NativeUUID();
			}
			UniversallyUniqueID ToUniversallyUniqueID() const noexcept;

			void FromPlatformUUID(const void* uuid) noexcept;
			void ToPlatformUUID(void* uuid) const noexcept;

		public:
			explicit constexpr operator bool() const noexcept
			{
				return !IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return IsNull();
			}

			constexpr bool operator==(const NativeUUID& other) const noexcept
			{
				if (this == &other)
				{
					return true;
				}
				else if (Data1 == other.Data1 && Data2 == other.Data2 && Data3 == other.Data3)
				{
					for (size_t i = 0; i < sizeof(NativeUUID::Data4); i++)
					{
						if (Data4[i] != other.Data4[i])
						{
							return false;
						}
					}
					return true;
				}
				return false;
			}
			constexpr bool operator!=(const NativeUUID& other) const noexcept
			{
				return !(*this == other);
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<NativeUUID> final
	{
		uint64_t Serialize(IOutputStream& stream, const NativeUUID& value) const;
		uint64_t Deserialize(IInputStream& stream, NativeUUID& value) const;
	};
}

namespace std
{
	template<>
	struct hash<kxf::NativeUUID>
	{
		constexpr size_t operator()(const kxf::NativeUUID& uuid) const noexcept
		{
			return uuid.GetHash();
		}
	};

	template<>
	struct numeric_limits<kxf::NativeUUID>
	{
		static constexpr bool is_specialized = true;
		static constexpr bool is_exact = true;
		static constexpr bool is_bounded = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_signed = false;
		static constexpr int radix = 2;

		static constexpr kxf::NativeUUID min() noexcept
		{
			return {};
		}
		static constexpr kxf::NativeUUID lowest() noexcept
		{
			return min();
		}
		static constexpr kxf::NativeUUID max() noexcept
		{
			return {0xffffffff, 0xffff, 0xffff, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
		}
	};
}
