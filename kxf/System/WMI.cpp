#include "stdafx.h"
#include "WMI.h"
#include "VariantProperty.h"
#include "kxf/Utility/Enumerator.h"
#include "kxf/Utility/ScopeGuard.h"
#include <Windows.h>
#include <wbemcli.h>
#include "UndefWindows.h"
#pragma comment(lib, "wbemuuid.lib")

#include "Private/BeginIncludeCOM.h"
namespace
{
	Kx_MakeWinUnicodeCallWrapper(FormatMessage);
}
#include <comdef.h>
#include "Private/EndIncludeCOM.h"

namespace
{
	using namespace kxf;

	constexpr long g_ConnectServerFlags = WBEM_FLAG_CONNECT_USE_MAX_WAIT;

	_bstr_t ToBSTR(const String& value)
	{
		return value.wc_str();
	}
	const BSTR StringOrNull(const _bstr_t& value) noexcept
	{
		if (value.length() != 0)
		{
			return static_cast<const BSTR>(value);
		}
		return nullptr;
	}

	COMPtr<IEnumWbemClassObject> GetWbemEnumerator(IWbemServices& service, const String& query)
	{
		COMPtr<IEnumWbemClassObject> wbemEnumerator;
		if (HResult result = service.ExecQuery(ToBSTR(wxS("WQL")), ToBSTR(query), WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &wbemEnumerator))
		{
			return wbemEnumerator;
		}
		return nullptr;
	}
}

namespace kxf
{
	bool WMINamespace::Initialize(const kxf::FSPath& wmiNamespace, const UserCredentials* credentials, const Locale& locale)
	{
		if (HResult result = COM::CreateInstance(CLSID_WbemLocator, ClassContext::InprocServer, &m_Locator))
		{
			auto ns = ToBSTR(wmiNamespace.GetFullPath());
			auto localeName = ToBSTR(locale.GetName());

			if (credentials)
			{
				auto userName = ToBSTR(credentials->GetName());
				if (credentials->HasSecret())
				{
					auto password = ToBSTR(credentials->GetSecret().ToString());
					result = m_Locator->ConnectServer(ns, userName, password, StringOrNull(localeName), g_ConnectServerFlags, nullptr, nullptr, &m_Service);
				}
				else
				{
					result = m_Locator->ConnectServer(ns, userName, nullptr, StringOrNull(localeName), g_ConnectServerFlags, nullptr, nullptr, &m_Service);
				}
			}
			else
			{
				result = m_Locator->ConnectServer(ns, nullptr, nullptr, StringOrNull(localeName), g_ConnectServerFlags, nullptr, nullptr, &m_Service);
			}

			if (result)
			{
				result = ::CoSetProxyBlanket(m_Service.Get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, 0, EOAC_NONE);
			}
			return result.IsSuccess();
		}
		return false;
	}

	WMINamespace::WMINamespace() noexcept = default;
	WMINamespace::WMINamespace(const FSPath& wmiNamespace, const Locale& locale)
	{
		Initialize(wmiNamespace, nullptr, locale);
	}
	WMINamespace::WMINamespace(const FSPath& wmiNamespace, const UserCredentials& credentials, const Locale& locale)
	{
		Initialize(wmiNamespace, &credentials, locale);
	}
	WMINamespace::WMINamespace(const WMINamespace&) noexcept = default;
	WMINamespace::WMINamespace(WMINamespace&&) noexcept = default;
	WMINamespace::~WMINamespace() = default;

	bool WMINamespace::IsNull() const noexcept
	{
		return m_Locator.IsNull() || m_Service.IsNull();
	}

	Enumerator<String> WMINamespace::EnumClassNames() const
	{
		return Utility::MakeConvertingEnumerator<String>([](WMIClassObject classObject, IEnumerator& enumerator) -> std::optional<String>
		{
			// Only return the class object if it has its 'dynamic' qualifier set ti 'true'
			if (classObject.GetQualifier(wxS("Dynamic")).QueryAs<bool>() == true)
			{
				String name = classObject.GetClassName();
				if (!name.IsEmpty())
				{
					return name;
				}
			}
			enumerator.SkipCurrent();
			return {};
		}, const_cast<WMINamespace&>(*this), &WMINamespace::SelectAll, wxS("meta_class"));
	}
	Enumerator<String> WMINamespace::EnumChildNamespaces() const
	{
		return Utility::MakeConvertingEnumerator<String>([](WMIClassObject classObject, IEnumerator& enumerator) -> std::optional<String>
		{
			String name = classObject.GetName();
			if (!name.IsEmpty())
			{
				return name;
			}
			else
			{
				enumerator.SkipCurrent();
			}
			return {};
		}, const_cast<WMINamespace&>(*this), &WMINamespace::SelectAll, wxS("__NAMESPACE"));
	}

	Enumerator<WMIClassObject> WMINamespace::ExecuteQuery(const kxf::String& query)
	{
		if (auto wbemEnumerator = GetWbemEnumerator(*m_Service, query))
		{
			return [wbemEnumerator = std::move(wbemEnumerator)]() -> std::optional<WMIClassObject>
			{
				COMPtr<IWbemClassObject> classObject;
				ULONG count = 0;

				HResult result = wbemEnumerator->Next(WBEM_INFINITE, 1, &classObject, &count);
				if (result && count != 0)
				{
					return std::move(classObject);
				}
				return {};
			};
		}
		return {};
	}
	Enumerator<WMIClassObject> WMINamespace::SelectAll(const kxf::String& fromLocation)
	{
		// TODO: Add input sanitization of some kind?
		return ExecuteQuery(String::Format(wxS("SELECT * FROM %1"), fromLocation));
	}

	WMINamespace& WMINamespace::operator=(const WMINamespace&) noexcept = default;
	WMINamespace& WMINamespace::operator=(WMINamespace&& other) noexcept = default;
}
namespace kxf
{
	WMIClassObject::WMIClassObject() noexcept = default;
	WMIClassObject::WMIClassObject(COMPtr<IWbemClassObject> classObject) noexcept
		:m_ClassObject(std::move(classObject))
	{
	}

	bool WMIClassObject::IsNull() const noexcept
	{
		return m_ClassObject.IsNull();
	}
	bool WMIClassObject::IsSameAs(const WMIClassObject& other, FlagSet<WMIClassObjectComparisonFlag> flags) const noexcept
	{
		if (this == &other || IsNull() && other.IsNull())
		{
			return true;
		}
		else
		{
			auto MapFlags = [](FlagSet<WMIClassObjectComparisonFlag> flags) constexpr noexcept
			{
				FlagSet<long> nativeFlags;
				nativeFlags.Add(WBEM_FLAG_IGNORE_OBJECT_SOURCE, flags & WMIClassObjectComparisonFlag::IgnoreObjectSource);
				nativeFlags.Add(WBEM_FLAG_IGNORE_DEFAULT_VALUES, flags & WMIClassObjectComparisonFlag::IgnoreDefaultValues);
				nativeFlags.Add(WBEM_FLAG_IGNORE_QUALIFIERS, flags & WMIClassObjectComparisonFlag::IgnoreQualifiers);
				nativeFlags.Add(WBEM_FLAG_IGNORE_FLAVOR, flags & WMIClassObjectComparisonFlag::IgnoreFlavor);
				nativeFlags.Add(WBEM_FLAG_IGNORE_CLASS, flags & WMIClassObjectComparisonFlag::IgnoreClass);
				nativeFlags.Add(WBEM_FLAG_IGNORE_CASE, flags & WMIClassObjectComparisonFlag::IgnoreCase);

				return *nativeFlags;
			};
			return HResult(m_ClassObject->CompareTo(MapFlags(flags), &const_cast<IWbemClassObject&>(*other.m_ClassObject))).IsSuccess();
		}
	}

	COMPtr<IWbemClassObject> WMIClassObject::Get() const noexcept
	{
		return m_ClassObject;
	}

	String WMIClassObject::GetName() const
	{
		return GetProperty(wxS("Name")).GetAs<String>();
	}
	String WMIClassObject::GetClassName() const
	{
		return GetProperty(wxS("__Class")).GetAs<String>();
	}
	Any WMIClassObject::GetProperty(const kxf::String& name) const
	{
		VariantProperty property;
		if (HResult(m_ClassObject->Get(name.wc_str(), WBEM_FLAG_ALWAYS, reinterpret_cast<VARIANT*>(&property), nullptr, nullptr)))
		{
			return property.ToAny();
		}
		return {};
	}

	Enumerator<std::pair<String, kxf::Any>> WMIClassObject::EnumQualifiers() const
	{
		COMPtr<IWbemQualifierSet> qualifierSet;
		if (HResult result = m_ClassObject->GetQualifierSet(&qualifierSet))
		{
			return [qualifierSet = std::move(qualifierSet)]() -> std::optional<std::pair<String, kxf::Any>>
			{
				BSTRPtr qualifierName;
				VariantProperty qualifierValue;
				if (HResult result = qualifierSet->Next(0, &qualifierName, reinterpret_cast<VARIANT*>(&qualifierValue), nullptr))
				{
					return std::make_pair(qualifierName.Get(), qualifierValue.ToAny());
				}
				return {};
			};
		}
		return {};
	}
	Enumerator<String> WMIClassObject::EnumQualifierNames(FlagSet<WMIClassObjectFlag> flags) const
	{
		COMPtr<IWbemQualifierSet> qualifierSet;
		if (HResult result = m_ClassObject->GetQualifierSet(&qualifierSet))
		{
			auto MapFlags = [](FlagSet<WMIClassObjectFlag> flags) constexpr noexcept
			{
				FlagSet<long> nativeFlags;
				nativeFlags.Add(WBEM_FLAG_LOCAL_ONLY, flags & WMIClassObjectFlag::LocalOnly);
				nativeFlags.Add(WBEM_FLAG_PROPAGATED_ONLY, flags & WMIClassObjectFlag::PropagatedOnly);

				return *nativeFlags;
			};

			SafeArrayPtr items;
			if (result = qualifierSet->GetNames(MapFlags(flags), &items))
			{
				LONG lower = 0;
				LONG upper = 0;
				result = ::SafeArrayGetLBound(items.Get(), 1, &lower);
				result = ::SafeArrayGetUBound(items.Get(), 1, &upper);

				if (result)
				{
					return Utility::MakeEnumerator([qualifierSet = std::move(qualifierSet), items = std::move(items), index = 0_zu]() mutable -> std::optional<String>
					{
						void* data = nullptr;
						if (HResult(::SafeArrayAccessData(items.Get(), &data)))
						{
							Utility::ScopeGuard atExit = [&]()
							{
								::SafeArrayUnaccessData(items.Get());
							};
							return reinterpret_cast<BSTR>(data)[index++];
						}
						return {};
					}, upper - lower);
				}
			}
		}
		return {};
	}
	Any WMIClassObject::GetQualifier(const kxf::String& name) const
	{
		COMPtr<IWbemQualifierSet> qualifierSet;
		if (HResult result = m_ClassObject->GetQualifierSet(&qualifierSet))
		{
			VariantProperty qualifierValue;
			if (result = qualifierSet->Get(name.wc_str(), 0, reinterpret_cast<VARIANT*>(&qualifierValue), nullptr))
			{
				return qualifierValue.ToAny();
			}
		}
		return {};
	}

	WMIClassObject::WMIClassObject(const WMIClassObject&) noexcept = default;
	WMIClassObject::WMIClassObject(WMIClassObject&&) noexcept = default;
	WMIClassObject::~WMIClassObject() = default;

	WMIClassObject& WMIClassObject::operator=(const WMIClassObject&) noexcept = default;
	WMIClassObject& WMIClassObject::operator=(WMIClassObject&& other) noexcept = default;
}
