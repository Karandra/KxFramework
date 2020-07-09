#pragma once
#include "Common.h"
#include "kxf/General/StdID.h"
#include "kxf/Utility/Numeric.h"

namespace kxf
{
	class WidgetID final
	{
		private:
			static constexpr int MakeInt(StdID id) noexcept
			{
				return static_cast<int>(id);
			}

		private:
			StdID m_ID = StdID::None;

		public:
			constexpr WidgetID() noexcept = default;
			constexpr WidgetID(StdID id) noexcept
				:m_ID(id)
			{
			}
			constexpr WidgetID(int id) noexcept
				:m_ID(static_cast<StdID>(id))
			{
			}

		public:
			constexpr bool IsAny() const noexcept
			{
				return m_ID == StdID::Any;
			}
			constexpr bool IsNone() const noexcept
			{
				return m_ID == StdID::None;
			}
			constexpr bool IsSeparator() const noexcept
			{
				return m_ID == StdID::Separator;
			}

			constexpr bool IsAutomatic() const noexcept
			{
				return Utility::TestRange(MakeInt(m_ID), MakeInt(StdID::WX_AUTO_LOWEST), MakeInt(StdID::WX_AUTO_HIGHEST));
			}
			constexpr bool IsStandard() const noexcept
			{
				return IsWxStandard() || Utility::TestRange(MakeInt(m_ID), MakeInt(StdID::KX_LOWEST), MakeInt(StdID::KX_HIGHEST));
			}
			constexpr bool IsWxStandard() const noexcept
			{
				return Utility::TestRange(MakeInt(m_ID), MakeInt(StdID::WX_LOWEST), MakeInt(StdID::WX_HIGHEST));
			}
			constexpr bool IsUserDefined() const noexcept
			{
				return !IsAutomatic() && !IsStandard();
			}

			constexpr int operator*() const noexcept
			{
				return MakeInt(m_ID);
			}

		public:
			constexpr bool operator==(const WidgetID& other) const noexcept
			{
				return m_ID == other.m_ID;
			}
			constexpr bool operator!=(const WidgetID& other) const noexcept
			{
				return m_ID != other.m_ID;
			}
	};
}
