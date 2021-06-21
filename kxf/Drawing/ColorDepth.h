#pragma once
#include "Common.h"
#include "kxf/Serialization/BinarySerializer.h"

namespace kxf
{
	class ColorDepth final
	{
		friend struct BinarySerializer<ColorDepth>;

		private:
			int m_Value = 0;

		public:
			constexpr ColorDepth() noexcept = default;
			constexpr ColorDepth(int value) noexcept
				:m_Value(value > 0 ? value : 0)
			{
			}

		public:
			constexpr bool IsNull() const noexcept
			{
				return m_Value == 0;
			}
			constexpr int GetValue() const noexcept
			{
				return m_Value;
			}

		public:
			constexpr explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return IsNull();
			}

			constexpr auto operator<=>(const ColorDepth&) const noexcept = default;
			constexpr int operator*() const noexcept
			{
				return m_Value;
			}
	};
}

namespace kxf::ColorDepthDB
{
	constexpr ColorDepth BPP1 = {1};
	constexpr ColorDepth BPP8 = {8};
	constexpr ColorDepth BPP16 = {16};
	constexpr ColorDepth BPP24 = {24};
	constexpr ColorDepth BPP32 = {32};
	constexpr ColorDepth BPP48 = {48};
}

namespace kxf
{
	template<>
	struct BinarySerializer<ColorDepth> final
	{
		uint64_t Serialize(IOutputStream& stream, const ColorDepth& value) const
		{
			return Serialization::WriteObject(stream, value.m_Value);
		}
		uint64_t Deserialize(IInputStream& stream, ColorDepth& value) const
		{
			return Serialization::ReadObject(stream, value.m_Value);
		}
	};
}
