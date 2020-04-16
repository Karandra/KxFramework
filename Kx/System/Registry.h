#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include "Kx/FileSystem/FSPath.h"

namespace KxFramework
{
	enum class RegistryBaseKey
	{
		LocalMachine,
		Users,
		ClassesRoot,
		CurrentUser,
		CurrentUserLocalSettings,
		CurrentConfig,
		PerformanceData,
		PerformanceText,
		PerformanceTextNLS,
		DynData,
	};
	enum class RegistryAccess
	{
		None = 0,

		Read = 1 << 0,
		Write = 1 << 1,
		Create = 1 << 2,
		Delete = 1 << 3,
		Notify = 1 << 4,
		Enumerate = 1 << 5,

		Everything = Create|Notify|Enumerate|Read|Write
	};
	enum class RegistryKeyFlag
	{
		None = 0,
		Volatile = 1 << 0
	};
	enum class RegistryWOW64
	{
		Default = 0,
		Access32,
		Access64
	};
	enum class RegistryValueType
	{
		Any = -1,
		None = 0,

		String,
		StringArray,
		StringExpand,

		SymLink,
		Binary,

		UInt32,
		UInt32_LE = UInt32,
		UInt32_BE,

		UInt64,
		UInt64_LE = UInt64,
		UInt64_BE,

		ResourceList,
		ResourceRequirementsList,
		FullResourceDescriptor,
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(RegistryAccess);
		Kx_EnumClass_AllowEverything(RegistryKeyFlag);
	}
}

namespace KxFramework
{
	class RegistryKey final
	{
		public:
			static RegistryKey CreateKey(RegistryBaseKey baseKey, const FSPath& subKey, RegistryAccess access, RegistryKeyFlag flags = RegistryKeyFlag::None, RegistryWOW64 wow64 = RegistryWOW64::Default)
			{
				RegistryKey key(baseKey, {}, RegistryAccess::Create, wow64);
				if (key)
				{
					return key.CreateKey(subKey, access, flags, wow64);
				}
				return {};
			}

		private:
			void* m_Handle = nullptr;

		private:
			void* DoGetBaseKey(RegistryBaseKey baseKey) const noexcept;
			bool DoOpenKey(void* rootKey, const FSPath& subKey, RegistryAccess access, RegistryWOW64 wow64);
			bool DoCreateKey(void* rootKey, const FSPath& subKey, RegistryAccess access, RegistryKeyFlag flags, RegistryWOW64 wow64);
			void DoCloseKey(void* handle) noexcept;

		public:
			RegistryKey() noexcept = default;
			RegistryKey(RegistryBaseKey baseKey) noexcept
				:m_Handle(DoGetBaseKey(baseKey))
			{
			}
			RegistryKey(RegistryBaseKey baseKey, const FSPath& subKey, RegistryAccess access, RegistryWOW64 wow64 = RegistryWOW64::Default)
			{
				DoOpenKey(DoGetBaseKey(baseKey), subKey, access, wow64);
			}
			RegistryKey(const RegistryKey&) = delete;
			RegistryKey(RegistryKey&& other) noexcept
			{
				*this = std::move(other);
			}
			~RegistryKey() noexcept
			{
				DoCloseKey(m_Handle);
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Handle == nullptr;
			}
			bool IsBaseKey() const noexcept;
			void Close() noexcept
			{
				DoCloseKey(m_Handle);
				m_Handle = nullptr;
			}

			RegistryKey& AttachHandle(void* handle) noexcept
			{
				DoCloseKey(m_Handle);
				m_Handle = handle;

				return *this;
			}
			void* DetachHandle() noexcept
			{
				void* handle = m_Handle;
				m_Handle = nullptr;
				return handle;
			}

			RegistryKey OpenKey(const FSPath& subKey, RegistryAccess access, RegistryWOW64 wow64 = RegistryWOW64::Default)
			{
				RegistryKey key;
				if (key.DoOpenKey(m_Handle, subKey, access, wow64))
				{
					return key;
				}
				return {};
			}
			RegistryKey CreateKey(const FSPath& subKey, RegistryAccess access, RegistryKeyFlag flags = RegistryKeyFlag::None, RegistryWOW64 wow64 = RegistryWOW64::Default)
			{
				RegistryKey key;
				if (key.DoCreateKey(m_Handle, subKey, access, flags, wow64))
				{
					return key;
				}
				return {};
			}
			bool RemoveKey(const FSPath& subKey, bool resursive = false);

			bool RemoveValue(const String& valueName);
			bool DoesValueExist(const String& valueName) const;
			RegistryValueType GetValueType(const String& valueName) const;

			size_t EnumKeyNames(std::function<bool(String)> func) const;
			size_t EnumValueNames(std::function<bool(String)> func) const;

			std::optional<String> GetStringValue(const String& valueName) const;
			bool SetStringValue(const String& valueName, const String& value);

			std::optional<String> GetStringExpandValue(const String& valueName, bool noAutoExpand = false) const;
			bool SetStringExpandValue(const String& valueName, const String& value);

			size_t GetStringArrayValue(const String& valueName, std::function<bool(String)> func) const;
			bool SetStringArrayValue(const String& valueName, std::function<String()> func);

			std::optional<std::vector<uint8_t>> GetBinaryValue(const String& valueName) const;
			bool SetBinaryValue(const String& valueName, const void* data, size_t size);

			std::optional<uint32_t> GetUInt32Value(const String& valueName) const;
			bool SetUInt32Value(const String& valueName, uint32_t value, bool asBigEndian = false);

			std::optional<uint64_t> GetUInt64Value(const String& valueName) const;
			bool SetUInt64Value(const String& valueName, uint64_t value, bool asBigEndian = false);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			RegistryKey& operator=(RegistryKey&& other) noexcept
			{
				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;

				return *this;
			}
			RegistryKey& operator=(const RegistryKey&) = delete;
	};
}
