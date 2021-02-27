#pragma once
#include "Common.h"
#include "COM.h"
#include "kxf/General/Any.h"
#include "kxf/General/String.h"
#include "kxf/General/Enumerator.h"
#include "kxf/Crypto/UserCredentials.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Localization/Locale.h"
struct IWbemLocator;
struct IWbemServices;
struct IWbemClassObject;

namespace kxf
{
	class WMIClassObject;
}

namespace kxf
{
	class KX_API WMINamespace final
	{
		private:
			COMPtr<IWbemLocator> m_Locator;
			COMPtr<IWbemServices> m_Service;

		private:
			bool Initialize(const kxf::FSPath& wmiNamespace, const UserCredentials* credentials = nullptr, const Locale& locale = {});

		public:
			WMINamespace() noexcept;
			WMINamespace(const FSPath& wmiNamespace, const Locale& locale = {});
			WMINamespace(const FSPath& wmiNamespace, const UserCredentials& credentials, const Locale& locale = {});
			WMINamespace(const WMINamespace&) noexcept;
			WMINamespace(WMINamespace&&) noexcept;
			~WMINamespace();

		public:
			bool IsNull() const noexcept;

			Enumerator<String> EnumClassNames() const;
			Enumerator<String> EnumChildNamespaces() const;

			Enumerator<WMIClassObject> ExecuteQuery(const kxf::String& query);
			Enumerator<WMIClassObject> SelectAll(const kxf::String& fromLocation);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			WMINamespace& operator=(WMINamespace&& other) noexcept;
			WMINamespace& operator=(const WMINamespace&) noexcept;
	};
}
namespace kxf
{
	enum class WMIClassObjectFlag: uint32_t
	{
		None = 0,

		LocalOnly = 1 << 0,
		PropagatedOnly = 1 << 1
	};
	KxFlagSet_Declare(WMIClassObjectFlag);

	enum class WMIClassObjectComparisonFlag: uint32_t
	{
		None = 0,

		IgnoreObjectSource = 1 << 0,
		IgnoreDefaultValues = 1 << 1,
		IgnoreQualifiers = 1 << 2,
		IgnoreFlavor = 1 << 3,
		IgnoreClass = 1 << 4,
		IgnoreCase = 1 << 5
	};
	KxFlagSet_Declare(WMIClassObjectComparisonFlag);

	class KX_API WMIClassObject final
	{
		friend class WMINamespace;

		private:
			COMPtr<IWbemClassObject> m_ClassObject;

		public:
			WMIClassObject() noexcept;
			WMIClassObject(COMPtr<IWbemClassObject> classObject) noexcept;
			WMIClassObject(const WMIClassObject&) noexcept;
			WMIClassObject(WMIClassObject&&) noexcept;
			~WMIClassObject();

		public:
			bool IsNull() const noexcept;
			bool IsSameAs(const WMIClassObject& other, FlagSet<WMIClassObjectComparisonFlag> flags = {}) const noexcept;

			COMPtr<IWbemClassObject> Get() const noexcept;

			String GetName() const;
			String GetClassName() const;
			Any GetProperty(const kxf::String& name) const;

			Enumerator<std::pair<String, Any>> EnumQualifiers() const;
			Enumerator<String> EnumQualifierNames(FlagSet<WMIClassObjectFlag> flags = {}) const;
			Any GetQualifier(const kxf::String& name) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			bool operator==(const WMIClassObject& other) const noexcept
			{
				return IsSameAs(other);
			}
			bool operator!=(const WMIClassObject& other) const noexcept
			{
				return !IsSameAs(other);
			}

			WMIClassObject& operator=(WMIClassObject&& other) noexcept;
			WMIClassObject& operator=(const WMIClassObject&) noexcept;
	};
}
