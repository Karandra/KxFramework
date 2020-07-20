#include "stdafx.h"
#include "EventID.h"
#include <wx/event.h>

namespace kxf
{
	bool EventID::IsNull() const noexcept
	{
		if (m_ID.valueless_by_exception() || m_ID.index() == std::numeric_limits<size_t>::max())
		{
			return true;
		}
		else if (auto value = std::get_if<int64_t>(&m_ID))
		{
			return *value == 0;
		}
		else if (auto value = std::get_if<UniversallyUniqueID>(&m_ID))
		{
			return value->IsNull();
		}
		else if (auto value = std::get_if<String>(&m_ID))
		{
			return value->IsEmpty();
		}
		return false;
	}
	size_t EventID::GetHash() const noexcept
	{
		if (auto value = std::get_if<int64_t>(&m_ID))
		{
			if (*value == 0)
			{
				return std::hash<int64_t>()(*value);
			}
		}
		else if (auto value = std::get_if<UniversallyUniqueID>(&m_ID))
		{
			if (value->IsNull())
			{
				return std::hash<UniversallyUniqueID>()(*value);
			}
		}
		else if (auto value = std::get_if<String>(&m_ID))
		{
			if (value->IsEmpty())
			{
				return std::hash<String>()(*value);
			}
		}
		return 0;
	}

	int64_t EventID::AsInt() const noexcept
	{
		if (auto value = std::get_if<int64_t>(&m_ID))
		{
			return *value;
		}
		return 0;
	}
	UniversallyUniqueID EventID::AsUniqueID() const noexcept
	{
		if (auto value = std::get_if<UniversallyUniqueID>(&m_ID))
		{
			return *value;
		}
		return {};
	}
	const String& EventID::AsString() const noexcept
	{
		if (auto value = std::get_if<String>(&m_ID))
		{
			return *value;
		}
		return NullString;
	}
}

namespace kxf::EventSystem
{
	EventID NewSimpleEventID() noexcept
	{
		return static_cast<int64_t>(::wxNewEventType());
	}
	EventID NewUniqueEventID() noexcept
	{
		return UniversallyUniqueID::CreateSequential();
	}
}
