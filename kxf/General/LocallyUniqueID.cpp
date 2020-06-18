#include "stdafx.h"
#include "LocallyUniqueID.h"
#include <cwchar>

namespace
{
	std::atomic<uint64_t> g_SequentialID = 0;

	uint64_t CreateFromString(const wchar_t* value) noexcept
	{
		wchar_t* end = nullptr;
		return std::wcstoull(value, &end, 16);
	}
	uint64_t CreateFromString(const char* value) noexcept
	{
		char* end = nullptr;
		return std::strtoull(value, &end, 16);
	}
}

namespace kxf
{
	LocallyUniqueID LocallyUniqueID::CreateSequential() noexcept
	{
		return ++g_SequentialID;
	}

	LocallyUniqueID::LocallyUniqueID(const char* value) noexcept
		:m_ID(CreateFromString(value))
	{
	}
	LocallyUniqueID::LocallyUniqueID(const wchar_t* value) noexcept
		:m_ID(CreateFromString(value))
	{
	}
	LocallyUniqueID::LocallyUniqueID(const String& value) noexcept
		:m_ID(CreateFromString(value.wx_str()))
	{
	}

	String LocallyUniqueID::ToString() const
	{
		return !IsNull() ? String::Format(wxS("%1"), m_ID) : NullString;
	}
}
