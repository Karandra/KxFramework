#include "stdafx.h"
#include "ClassInfo.h"
#include "kxf/General/String.h"

namespace
{
	kxf::RTTI::ClassInfo* m_FirstClassInfo = nullptr;

	std::string_view DoGetFullyQualifiedName(std::string_view sourceName, size_t index)
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
	std::string_view DoGetNamePart(std::string_view fullyQualifiedName, NamePart desiredName)
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
	const ClassInfo* ClassInfo::GetFirst() noexcept
	{
		return m_FirstClassInfo;
	}

	void ClassInfo::OnCreate() noexcept
	{
		m_Next = m_FirstClassInfo;
		m_FirstClassInfo = this;
	}
	void ClassInfo::OnDestroy() noexcept
	{
		m_FirstClassInfo = m_Next;
	}

	void ClassInfo::Initialize(std::string_view name, size_t size, size_t alignment, FlagSet<ClassTrait> traits, const std::type_info& typeInfo)
	{
		m_FullyQualifiedName = DoGetFullyQualifiedName(name, 0);
		m_Size = size;
		m_Alignment = alignment;
		m_Traits = traits;
		m_TypeInfo = &typeInfo;
	}
	bool ClassInfo::IsInitialized() const
	{
		return m_TypeInfo != nullptr;
	}

	// IObject
	RTTI::QueryInfo ClassInfo::DoQueryInterface(const kxf::IID& iid) noexcept
	{
		return nullptr;
	}

	std::string_view ClassInfo::ParseToFullyQualifiedName(std::string_view name, size_t index) const
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
}
