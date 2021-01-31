#include "stdafx.h"
#include "Common.h"
#include "ClassInfo.h"
#include "kxf/General/String.h"

namespace kxf::RTTI
{
	size_t EnumClassInfo(std::function<bool(const ClassInfo&)> func) noexcept
	{
		size_t count = 0;
		for (const ClassInfo* classInfo = ClassInfo::GetFirst(); classInfo; classInfo = classInfo->GetNext())
		{
			count++;
			if (func && !std::invoke(func, *classInfo))
			{
				break;
			}
		}
		return count;
	}

	const ClassInfo* GetClassInfoByName(const char* fullyQualifiedName) noexcept
	{
		return GetClassInfoByName(std::string_view(fullyQualifiedName));
	}
	const ClassInfo* GetClassInfoByName(std::string_view fullyQualifiedName) noexcept
	{
		const ClassInfo* result = nullptr;
		EnumClassInfo([&](const ClassInfo& classInfo)
		{
			if (classInfo.m_FullyQualifiedName == fullyQualifiedName)
			{
				result = &classInfo;
			}
			return result == nullptr;
		});

		return result;
	}
	const ClassInfo* GetClassInfoByName(const kxf::String& fullyQualifiedName) noexcept
	{
		const ClassInfo* result = nullptr;
		EnumClassInfo([&](const ClassInfo& classInfo)
		{
			if (fullyQualifiedName.IsSameAs(classInfo.m_FullyQualifiedName))
			{
				result = &classInfo;
			}
			return result == nullptr;
		});

		return result;
	}
}
