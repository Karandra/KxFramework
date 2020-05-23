#include "stdafx.h"
#include "Registry.h"
#include "Kx/Utility/Common.h"
#include <Windows.h>
#include "UndefWindows.h"

namespace
{
	using namespace KxFramework;

	constexpr size_t g_MaxKeyNameLength = std::numeric_limits<uint8_t>::max() + 1;
	constexpr size_t g_MaxValueNameLength = std::numeric_limits<int16_t>::max() + 1;

	HKEY AsHKEY(void* handle) noexcept
	{
		return reinterpret_cast<HKEY>(handle);
	}

	HKEY MapBaseKey(RegistryBaseKey baseKey) noexcept
	{
		switch (baseKey)
		{
			case RegistryBaseKey::LocalMachine:
			{
				return HKEY_LOCAL_MACHINE;
			}
			case RegistryBaseKey::Users:
			{
				return HKEY_USERS;
			}
			case RegistryBaseKey::ClassesRoot:
			{
				return HKEY_CLASSES_ROOT;
			}
			case RegistryBaseKey::CurrentUser:
			{
				return HKEY_CURRENT_USER;
			}
			case RegistryBaseKey::CurrentUserLocalSettings:
			{
				return HKEY_CURRENT_USER_LOCAL_SETTINGS;
			}
			case RegistryBaseKey::CurrentConfig:
			{
				return HKEY_CURRENT_CONFIG;
			}
			case RegistryBaseKey::PerformanceData:
			{
				return HKEY_PERFORMANCE_DATA;
			}
			case RegistryBaseKey::PerformanceText:
			{
				return HKEY_PERFORMANCE_TEXT;
			}
			case RegistryBaseKey::PerformanceTextNLS:
			{
				return HKEY_PERFORMANCE_NLSTEXT;
			}
			case RegistryBaseKey::DynData:
			{
				return HKEY_DYN_DATA;
			}
		};
		return nullptr;
	}
	REGSAM MapAccessMode(FlagSet<RegistryAccess> access) noexcept
	{
		using namespace KxFramework;

		if (access == RegistryAccess::Everything)
		{
			return KEY_ALL_ACCESS;
		}
		else
		{
			REGSAM nativeAccess = 0;
			Utility::AddFlagRef(nativeAccess, KEY_READ, access & RegistryAccess::Read);
			Utility::AddFlagRef(nativeAccess, KEY_WRITE, access & RegistryAccess::Write);
			Utility::AddFlagRef(nativeAccess, DELETE, access & RegistryAccess::Delete);
			Utility::AddFlagRef(nativeAccess, KEY_NOTIFY, access & RegistryAccess::Notify);
			Utility::AddFlagRef(nativeAccess, KEY_CREATE_SUB_KEY, access & RegistryAccess::Create);
			Utility::AddFlagRef(nativeAccess, KEY_ENUMERATE_SUB_KEYS, access & RegistryAccess::Enumerate);

			return nativeAccess;
		}
	}
	REGSAM MapWOW64(RegistryWOW64 wow64) noexcept
	{
		using namespace KxFramework;

		switch (wow64)
		{
			case RegistryWOW64::Access32:
			{
				return KEY_WOW64_32KEY;
			}
			case RegistryWOW64::Access64:
			{
				return KEY_WOW64_64KEY;
			}
		}
		return 0;
	}
	
	DWORD MapValueType(RegistryValueType type) noexcept
	{
		switch (type)
		{
			case RegistryValueType::String:
			{
				return REG_SZ;
			}
			case RegistryValueType::StringExpand:
			{
				return REG_EXPAND_SZ;
			}
			case RegistryValueType::StringArray:
			{
				return REG_MULTI_SZ;
			}
			case RegistryValueType::SymLink:
			{
				return REG_LINK;
			}
			case RegistryValueType::Binary:
			{
				return REG_BINARY;
			}
			case RegistryValueType::UInt32_LE:
			{
				return REG_DWORD_LITTLE_ENDIAN;
			}
			case RegistryValueType::UInt32_BE:
			{
				return REG_DWORD_BIG_ENDIAN;
			}
			case RegistryValueType::UInt64_LE:
			case RegistryValueType::UInt64_BE:
			{
				// There is no REG_QWORD_BIG_ENDIAN
				return REG_QWORD_LITTLE_ENDIAN;
			}
			case RegistryValueType::ResourceList:
			{
				return REG_RESOURCE_LIST;
			}
			case RegistryValueType::ResourceRequirementsList:
			{
				return REG_RESOURCE_REQUIREMENTS_LIST;
			}
			case RegistryValueType::FullResourceDescriptor:
			{
				return REG_FULL_RESOURCE_DESCRIPTOR;
			}
		};
		return REG_NONE;
	}
	RegistryValueType MapValueType(DWORD type) noexcept
	{
		switch (type)
		{
			case REG_SZ:
			{
				return RegistryValueType::String;
			}
			case REG_EXPAND_SZ:
			{
				return RegistryValueType::StringExpand;
			}
			case REG_MULTI_SZ:
			{
				return RegistryValueType::StringArray;
			}
			case REG_DWORD_LITTLE_ENDIAN:
			{
				return RegistryValueType::UInt32_LE;
			}
			case REG_DWORD_BIG_ENDIAN:
			{
				return RegistryValueType::UInt32_BE;
			}
			case REG_QWORD_LITTLE_ENDIAN:
			{
				return RegistryValueType::UInt64_LE;
			}
			case REG_RESOURCE_LIST:
			{
				return RegistryValueType::ResourceList;
			}
			case REG_RESOURCE_REQUIREMENTS_LIST:
			{
				return RegistryValueType::ResourceRequirementsList;
			}
			case REG_FULL_RESOURCE_DESCRIPTOR:
			{
				return RegistryValueType::FullResourceDescriptor;
			}
		};
		return RegistryValueType::None;
	}

	template<class T>
	std::optional<T> DoGetNumericValue(HKEY hkey, const String& valueName, DWORD& actualType, Win32Error& lastError) noexcept
	{
		DWORD dataSize = 0;
		lastError = ::RegGetValueW(hkey, nullptr, valueName.wc_str(), RRF_RT_ANY, &actualType, nullptr, &dataSize);
		if (lastError.IsSuccess() && dataSize == sizeof(T))
		{
			T value = 0;
			if (lastError = ::RegGetValueW(hkey, nullptr, valueName.wc_str(), RRF_RT_ANY, &actualType, &value, &dataSize))
			{
				if constexpr(std::is_same_v<T, uint32_t>)
				{
					if (actualType == REG_DWORD_BIG_ENDIAN)
					{
						value = _byteswap_ulong(value);
					}
				}
				return value;
			}
		}
		return {};
	}

	std::optional<String> DoGetStringValue(HKEY hkey, const String& valueName, DWORD& actualType, Win32Error& lastError, DWORD desiredType, DWORD flags = 0)
	{
		DWORD dataSize = 0;
		lastError = ::RegGetValueW(hkey, nullptr, valueName.wc_str(), desiredType|flags, &actualType, nullptr, &dataSize);
		if (lastError && dataSize != 0)
		{
			String result;
			if (lastError = ::RegGetValueW(hkey, nullptr, valueName.wc_str(), desiredType|flags, &actualType, wxStringBuffer(result, dataSize - 1), &dataSize))
			{
				return result;
			}
		}
		return {};
	}
	bool DoSetStringValue(HKEY hkey, const String& valueName, const String& value, DWORD type, Win32Error& lastError)
	{
		lastError = ::RegSetKeyValueW(hkey, nullptr, valueName.wc_str(), type, value.wc_str(), value.length() * sizeof(wchar_t) + sizeof(wchar_t));
		return lastError.IsSuccess();
	}

	bool CheckIsBaseKey(HKEY handle) noexcept
	{
		if (handle)
		{
			return handle == HKEY_LOCAL_MACHINE ||
				handle == HKEY_USERS ||
				handle == HKEY_CLASSES_ROOT ||
				handle == HKEY_CURRENT_USER ||
				handle == HKEY_CURRENT_USER_LOCAL_SETTINGS ||
				handle == HKEY_CURRENT_CONFIG ||
				handle == HKEY_PERFORMANCE_DATA ||
				handle == HKEY_PERFORMANCE_TEXT ||
				handle == HKEY_PERFORMANCE_NLSTEXT ||
				handle == HKEY_DYN_DATA;
		}
		return false;
	}
}

namespace KxFramework
{
	void* RegistryKey::DoGetBaseKey(RegistryBaseKey baseKey) const noexcept
	{
		return MapBaseKey(baseKey);
	}
	bool RegistryKey::DoOpenKey(void* rootKey, const FSPath& subKey, FlagSet<RegistryAccess> access, RegistryWOW64 wow64)
	{
		if (rootKey)
		{
			HKEY handle = nullptr;
			const String subKeyPath = subKey.GetFullPath();
			if (m_LastError = ::RegOpenKeyExW(AsHKEY(rootKey), subKeyPath.wc_str(), 0, MapAccessMode(access)|MapWOW64(wow64), &handle))
			{
				m_Handle = handle;
				return true;
			}
		}
		return false;
	}
	bool RegistryKey::DoCreateKey(void* rootKey, const FSPath& subKey, FlagSet<RegistryAccess> access, FlagSet<RegistryKeyFlag> flags, RegistryWOW64 wow64)
	{
		if (rootKey)
		{
			DWORD nativeFlags = 0;
			Utility::AddFlag(nativeFlags, REG_OPTION_VOLATILE, flags & RegistryKeyFlag::Volatile);

			HKEY handle = nullptr;
			String subKeyPath = subKey.GetFullPath();
			if (m_LastError = ::RegCreateKeyExW(AsHKEY(rootKey), subKeyPath.wc_str(), 0, nullptr, nativeFlags, MapAccessMode(access)|MapWOW64(wow64), nullptr, &handle, nullptr))
			{
				m_Handle = handle;
				return true;
			}
		}
		return false;
	}
	void RegistryKey::DoCloseKey(void* handle) noexcept
	{
		if (handle && !CheckIsBaseKey(AsHKEY(handle)))
		{
			m_LastError = ::RegCloseKey(AsHKEY(handle));
		}
	}

	bool RegistryKey::IsBaseKey() const noexcept
	{
		return m_Handle && CheckIsBaseKey(AsHKEY(m_Handle));
	}
	
	bool RegistryKey::RemoveKey(const FSPath& subKey, bool resursive)
	{
		String subKeyPath = subKey.GetFullPath();
		if (resursive)
		{
			m_LastError = ::RegDeleteTreeW(AsHKEY(m_Handle), subKeyPath.wc_str());
		}
		else
		{
			m_LastError = ::RegDeleteKeyExW(AsHKEY(m_Handle), subKeyPath.wc_str(), 0, 0);
		}
		return m_LastError.IsSuccess();
	}
	bool RegistryKey::RemoveValue(const String& valueName)
	{
		m_LastError = ::RegDeleteKeyValueW(AsHKEY(m_Handle), nullptr, valueName.wc_str());
		return m_LastError.IsSuccess();
	}
	bool RegistryKey::DoesValueExist(const String& valueName) const
	{
		m_LastError = ::RegQueryValueExW(AsHKEY(m_Handle), valueName.wc_str(), nullptr, nullptr, nullptr, nullptr);
		return m_LastError.IsSuccess();
	}
	RegistryValueType RegistryKey::GetValueType(const String& valueName) const
	{
		DWORD actualType = REG_NONE;
		if (m_LastError = ::RegQueryValueExW(AsHKEY(m_Handle), valueName.wc_str(), nullptr, &actualType, nullptr, nullptr))
		{
			return MapValueType(actualType);
		}
		return RegistryValueType::None;
	}

	size_t RegistryKey::EnumKeyNames(std::function<bool(String)> func) const
	{
		DWORD keyCount = 0;
		m_LastError = ::RegQueryInfoKeyW(AsHKEY(m_Handle), nullptr, nullptr, nullptr, &keyCount, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
		if (m_LastError && keyCount != 0)
		{
			wchar_t keyNameBuffer[g_MaxKeyNameLength] = {};

			size_t count = 0;
			for (size_t i = 0; i < keyCount; i++)
			{
				DWORD keyNameLength = std::size(keyNameBuffer);
				if (m_LastError = ::RegEnumKeyExW(AsHKEY(m_Handle), i, keyNameBuffer, &keyNameLength, nullptr, nullptr, nullptr, nullptr))
				{
					count++;
					if (!std::invoke(func, String(keyNameBuffer, keyNameLength)))
					{
						break;
					}
				}
			}
			return count;
		}
		return 0;
	}
	size_t RegistryKey::EnumValueNames(std::function<bool(String)> func) const
	{
		DWORD valueCount = 0;
		m_LastError = ::RegQueryInfoKeyW(AsHKEY(m_Handle), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &valueCount, nullptr, nullptr, nullptr, nullptr);
		if (m_LastError && valueCount != 0)
		{
			wchar_t valueNameBuffer[g_MaxValueNameLength] = {};

			size_t count = 0;
			for (size_t i = 0; i < valueCount; i++)
			{
				DWORD valueNameLength = std::size(valueNameBuffer);
				if (m_LastError = ::RegEnumValueW(AsHKEY(m_Handle), i, valueNameBuffer, &valueNameLength, nullptr, nullptr, nullptr, nullptr))
				{
					count++;
					if (!std::invoke(func, String(valueNameBuffer, valueNameLength)))
					{
						break;
					}
				}
			}
			return count;
		}
		return 0;
	}

	std::optional<String> RegistryKey::GetStringValue(const String& valueName) const
	{
		DWORD actualType = REG_NONE;
		return DoGetStringValue(AsHKEY(m_Handle), valueName, actualType, m_LastError, RRF_RT_REG_SZ);
	}
	bool RegistryKey::SetStringValue(const String& valueName, const String& value)
	{
		return DoSetStringValue(AsHKEY(m_Handle), valueName, value, REG_SZ, m_LastError);
	}
	
	std::optional<String> RegistryKey::GetStringExpandValue(const String& valueName, bool noAutoExpand) const
	{
		DWORD actualType = REG_NONE;
		return DoGetStringValue(AsHKEY(m_Handle), valueName, actualType, m_LastError, RRF_RT_REG_MULTI_SZ, noAutoExpand ? RRF_NOEXPAND : 0);
	}
	bool RegistryKey::SetStringExpandValue(const String& valueName, const String& value)
	{
		return DoSetStringValue(AsHKEY(m_Handle), valueName, value, REG_EXPAND_SZ, m_LastError);
	}
	
	size_t RegistryKey::GetStringArrayValue(const String& valueName, std::function<bool(String)> func) const
	{
		DWORD actualType = REG_NONE;
		if (auto value = DoGetStringValue(AsHKEY(m_Handle), valueName, actualType, m_LastError, RRF_RT_REG_MULTI_SZ))
		{
			const size_t length = value->length();

			size_t count = 0;
			for (size_t i = 0; i < length;)
			{
				StringView view = value->wc_str() + i;
				i += view.length() + 1;

				if (!view.empty())
				{
					count++;
					if (!std::invoke(func, String::FromView(view)))
					{
						break;
					}
				}
			}
			return count;
		}
		return 0;
	}
	bool RegistryKey::SetStringArrayValue(const String& valueName, std::function<String()> func)
	{
		String multiValue;
		do
		{
			String value = std::invoke(func);
			if (!value.IsEmpty())
			{
				if (!multiValue.IsEmpty())
				{
					multiValue += L'\0';
				}
				multiValue += std::move(value);
			}
			else
			{
				break;
			}
		}
		while (true);

		if (!multiValue.IsEmpty())
		{
			multiValue += L'\0';
		}
		else
		{
			multiValue = L"\0\0";
		}

		return DoSetStringValue(AsHKEY(m_Handle), valueName, multiValue, REG_MULTI_SZ, m_LastError);
	}

	std::optional<std::vector<uint8_t>> RegistryKey::GetBinaryValue(const String& valueName) const
	{
		constexpr DWORD desiredType = RRF_RT_REG_BINARY;
		constexpr DWORD flags = 0;
		DWORD actualType = REG_NONE;
		DWORD dataSize = 0;

		m_LastError = ::RegGetValueW(AsHKEY(m_Handle), nullptr, valueName.wc_str(), desiredType|flags, &actualType, nullptr, &dataSize);
		if (m_LastError && dataSize != 0)
		{
			std::vector<uint8_t> buffer;
			buffer.resize(dataSize);

			if (m_LastError = ::RegGetValueW(AsHKEY(m_Handle), nullptr, valueName.wc_str(), desiredType|flags, &actualType, buffer.data(), &dataSize))
			{
				return buffer;
			}
		}
		return {};
	}
	bool RegistryKey::SetBinaryValue(const String& valueName, const void* data, size_t size)
	{
		m_LastError = ::RegSetKeyValueW(AsHKEY(m_Handle), nullptr, valueName.wc_str(), REG_BINARY, data, size);
		return m_LastError.IsSuccess();
	}

	std::optional<uint32_t> RegistryKey::GetUInt32Value(const String& valueName) const
	{
		DWORD actualType = REG_NONE;
		if (auto value = DoGetNumericValue<uint32_t>(AsHKEY(m_Handle), valueName, actualType, m_LastError) && (actualType == REG_DWORD_LITTLE_ENDIAN || actualType == REG_DWORD_BIG_ENDIAN))
		{
			return value;
		}
		return {};
	}
	bool RegistryKey::SetUInt32Value(const String& valueName, uint32_t value, bool asBigEndian)
	{
		if (asBigEndian)
		{
			value = _byteswap_ulong(value);
		}
		m_LastError = ::RegSetKeyValueW(AsHKEY(m_Handle), nullptr, valueName.wc_str(), REG_DWORD_LITTLE_ENDIAN, &value, sizeof(value));
		return m_LastError.IsSuccess();
	}

	std::optional<uint64_t> RegistryKey::GetUInt64Value(const String& valueName) const
	{
		DWORD actualType = REG_NONE;
		if (auto value = DoGetNumericValue<uint64_t>(AsHKEY(m_Handle), valueName, actualType, m_LastError) && actualType == REG_QWORD)
		{
			return value;
		}
		return {};
	}
	bool RegistryKey::SetUInt64Value(const String& valueName, uint64_t value, bool asBigEndian)
	{
		if (asBigEndian)
		{
			value = _byteswap_uint64(value);
		}
		m_LastError = ::RegSetKeyValueW(AsHKEY(m_Handle), nullptr, valueName.wc_str(), REG_QWORD, &value, sizeof(value));
		return m_LastError.IsSuccess();
	}
}
