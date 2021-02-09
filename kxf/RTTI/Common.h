#pragma once
#include "kxf/Common.hpp"
#include <typeinfo>
#include <type_traits>
#include <functional>

namespace kxf
{
	class IID;
	class IObject;

	class String;

	template<class T>
	class Enumerator;
}
namespace kxf::RTTI
{
	class ClassInfo;
}

namespace kxf::RTTI
{
	template<class T>
	constexpr IID GetInterfaceID() noexcept
	{
		return T::ms_IID;
	}

	template<class T>
	const RTTI::ClassInfo& GetClassInfo() noexcept
	{
		return T::ms_ClassInfo;
	}

	Enumerator<const ClassInfo&> EnumClassInfo() noexcept;

	const ClassInfo* GetClassInfoByInterfaceID(const IID& iid) noexcept;
	const ClassInfo* GetClassInfoByName(const char* fullyQualifiedName) noexcept;
	const ClassInfo* GetClassInfoByName(std::string_view fullyQualifiedName) noexcept;
	const ClassInfo* GetClassInfoByName(const kxf::String& fullyQualifiedName) noexcept;
}
