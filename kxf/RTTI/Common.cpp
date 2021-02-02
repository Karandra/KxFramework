#include "stdafx.h"
#include "Common.h"
#include "ClassInfo.h"
#include "kxf/General/String.h"

namespace
{
	template<class TFunc>
	const kxf::RTTI::ClassInfo* DoGetClassInfo(TFunc&& func) noexcept
	{
		using namespace kxf::RTTI;

		const ClassInfo* result = nullptr;
		EnumClassInfo([&](const ClassInfo& classInfo)
		{
			if (std::invoke(func, classInfo))
			{
				result = &classInfo;
			}
			return result == nullptr;
		});

		return result;
	}
}

namespace kxf::RTTI
{
	size_t EnumClassInfo(std::function<bool(const ClassInfo&)> func) noexcept
	{
		size_t count = 0;
		for (const ClassInfo* classInfo = ClassInfo::GetFirstClassInfo(); classInfo; classInfo = classInfo->GetNextClassInfo())
		{
			count++;
			if (func && !std::invoke(func, *classInfo))
			{
				break;
			}
		}
		return count;
	}

	const kxf::RTTI::ClassInfo* GetClassInfoByInterfaceID(const IID& iid) noexcept
	{
		if (iid)
		{
			return DoGetClassInfo([&](const ClassInfo& classInfo)
			{
				return classInfo.GetInterfaceID() == iid;
			});
		}
		return nullptr;
	}
	const ClassInfo* GetClassInfoByName(const char* fullyQualifiedName) noexcept
	{
		return GetClassInfoByName(std::string_view(fullyQualifiedName));
	}
	const ClassInfo* GetClassInfoByName(std::string_view fullyQualifiedName) noexcept
	{
		if (!fullyQualifiedName.empty())
		{
			return DoGetClassInfo([&](const ClassInfo& classInfo)
			{
				return classInfo.m_FullyQualifiedName == fullyQualifiedName;
			});
		}
		return nullptr;
	}
	const ClassInfo* GetClassInfoByName(const kxf::String& fullyQualifiedName) noexcept
	{
		if (!fullyQualifiedName.IsEmpty())
		{
			return DoGetClassInfo([&](const ClassInfo& classInfo)
			{
				return fullyQualifiedName.IsSameAs(classInfo.m_FullyQualifiedName);
			});
		}
		return nullptr;
	}
}
