#include "stdafx.h"
#include "ClassInfo.h"
#include "kxf/General/String.h"

namespace
{
	kxf::RTTI::ClassInfo* m_FirstClassInfo = nullptr;

	std::string_view DoGetFullyQualifiedName(std::string_view sourceName, size_t index) noexcept
	{
		constexpr auto npos = std::string_view::npos;

		// Examples:
		//	kxf::RTTI::ClassInfoOf<class kxf::ITimeClock,class kxf::IObject>::ClassInfoOf
		//	kxf::RTTI::ClassInfoOf<class kxf::EventSystem::SignalInvocationEvent<0,void (__cdecl Kortex::IGameInstance::*)(void)>,class kxf::BasicEvent,class kxf::ISignalInvocationEvent>::ClassInfoOf
		//	kxf::RTTI::ClassInfoOf<class Kortex::DefaultApplication,class Kortex::IApplication,class kxf::Application::ICommandLine>::ClassInfoOf

		// Extract the first listed class which is the actual class name, others are base classes
		size_t classBegin = sourceName.find("<class ");
		if (classBegin != npos)
		{
			// Advance for the requested base class name
			for (size_t i = 1; i <= index && classBegin != npos; i++)
			{
				// Offset by 1 to force search for next anchor
				classBegin = sourceName.find(",class ", classBegin + 1);
			}

			if (classBegin != npos)
			{
				// Search for next class in the list or for the end
				size_t classEnd = sourceName.find(",class ", classBegin + 1);
				if (classEnd == npos)
				{
					classEnd = sourceName.rfind(">::ClassInfoOf");
				}

				if (classEnd != npos)
				{
					// Now extract the name
					auto fullyQualifiedName = sourceName.substr(classBegin, classEnd - classBegin);
					fullyQualifiedName.remove_prefix(7);

					return fullyQualifiedName;
				}
			}
		}
		return {};
	}

	enum class NamePart
	{
		ClassName,
		Namespace,
	};
	std::string_view DoGetNamePart(std::string_view fullyQualifiedName, NamePart desiredName) noexcept
	{
		constexpr auto npos = std::string_view::npos;
		if (size_t namespaceIndex = fullyQualifiedName.rfind("::"); namespaceIndex != npos)
		{
			if (desiredName == NamePart::ClassName)
			{
				return fullyQualifiedName.substr(namespaceIndex + 2);
			}
			else if (desiredName == NamePart::Namespace)
			{
				return fullyQualifiedName.substr(0, namespaceIndex);
			}
		}
		else
		{
			if (desiredName == NamePart::ClassName)
			{
				return fullyQualifiedName;
			}
		}
		return {};
	}
}

namespace kxf::RTTI
{
	const ClassInfo* ClassInfo::GetFirstClassInfo() noexcept
	{
		return m_FirstClassInfo;
	}

	void ClassInfo::OnCreate() noexcept
	{
		m_NextClassInfo = m_FirstClassInfo;
		m_FirstClassInfo = this;
	}
	void ClassInfo::OnDestroy() noexcept
	{
		m_FirstClassInfo = m_NextClassInfo;
	}

	std::unique_ptr<IObject> ClassInfo::DoCreateImplementation(const IID& iid) const
	{
		if (m_Traits.Contains(ClassTrait::Interface) && iid)
		{
			std::unique_ptr<IObject> result;
			EnumImplementations([&](const ClassInfo& classInfo)
			{
				if (classInfo.GetTraits().Contains(ClassTrait::Implementation) && classInfo.GetIID() == iid)
				{
					result = classInfo.CreateObjectInstance();
				}
				return result == nullptr;
			});

			return result;
		}
		return nullptr;
	}
	std::unique_ptr<IObject> ClassInfo::DoCreateImplementation(const String& fullyQualifiedName) const
	{
		if (m_Traits.Contains(ClassTrait::Interface) && !fullyQualifiedName.IsEmpty())
		{
			std::unique_ptr<IObject> result;
			EnumImplementations([&](const ClassInfo& classInfo)
			{
				if (classInfo.GetTraits().Contains(ClassTrait::Implementation) && fullyQualifiedName.IsSameAs(classInfo.m_FullyQualifiedName))
				{
					result = classInfo.CreateObjectInstance();
				}
				return result == nullptr;
			});

			return result;
		}
		return nullptr;
	}
	std::unique_ptr<IObject> ClassInfo::DoCreateAnyImplementation() const
	{
		if (m_Traits.Contains(ClassTrait::Interface))
		{
			std::unique_ptr<IObject> result;
			EnumImplementations([&](const ClassInfo& classInfo)
			{
				if (classInfo.GetTraits().Contains(ClassTrait::Implementation))
				{
					result = classInfo.CreateObjectInstance();
				}
				return result == nullptr;
			});

			return result;
		}
		return nullptr;
	}

	// IObject
	RTTI::QueryInfo ClassInfo::DoQueryInterface(const IID& iid) noexcept
	{
		return nullptr;
	}

	std::string_view ClassInfo::ParseToFullyQualifiedName(std::string_view name, size_t index) const noexcept
	{
		return DoGetFullyQualifiedName(name, index);
	}

	String ClassInfo::GetClassName() const
	{
		return String::FromView(DoGetNamePart(m_FullyQualifiedName, NamePart::ClassName));
	}
	kxf::String ClassInfo::GetNamespace() const
	{
		return String::FromView(DoGetNamePart(m_FullyQualifiedName, NamePart::Namespace));
	}
	String ClassInfo::GetFullyQualifiedName() const
	{
		return String::FromView(m_FullyQualifiedName);
	}

	size_t ClassInfo::EnumDerivedClasses(std::function<bool(const ClassInfo&)> func) const noexcept
	{
		size_t count = 0;
		for (const ClassInfo* classInfo = m_FirstClassInfo; classInfo; classInfo = classInfo->m_NextClassInfo)
		{
			if (IsBaseOf(*classInfo))
			{
				count++;
				if (func && !std::invoke(func, *classInfo))
				{
					break;
				}
			}
		}
		return count;
	}
	size_t ClassInfo::EnumImplementations(std::function<bool(const ClassInfo&)> func) const noexcept
	{
		size_t count = 0;
		EnumDerivedClasses([&](const ClassInfo& classInfo)
		{
			if (classInfo.GetTraits().Contains(ClassTrait::Implementation))
			{
				count++;
				return !func || std::invoke(func, classInfo);
			}
			return true;
		});
		return count;
	}
	size_t ClassInfo::EnumDerivedInterfaces(std::function<bool(const ClassInfo&)> func) const noexcept
	{
		size_t count = 0;
		EnumDerivedClasses([&](const ClassInfo& classInfo)
		{
			if (classInfo.GetTraits().Contains(ClassTrait::Interface))
			{
				count++;
				return !func || std::invoke(func, classInfo);
			}
			return true;
		});
		return count;
	}

	bool ClassInfo::IsBaseOf(const ClassInfo& other) const noexcept
	{
		bool result = false;
		other.EnumBaseClasses([&](const ClassInfo& classInfo)
		{
			if (classInfo == *this)
			{
				result = true;
				return false;
			}
			return true;
		});
		return result;
	}
	bool ClassInfo::IsSameAs(const ClassInfo& other) const noexcept
	{
		if (this == &other)
		{
			return true;
		}
		else
		{
			return m_Size == other.m_Size && m_Alignment == other.m_Alignment && m_Traits == other.m_Traits && m_FullyQualifiedName == other.m_FullyQualifiedName;
		}
	}
	bool ClassInfo::IsNull() const noexcept
	{
		return m_TypeInfo == nullptr;
	}
}
