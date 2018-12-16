/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxRegistry.h"
#include "KxFramework/KxUtility.h"

bool KxRegistry::IsTypesConform(DWORD realType, KxRegistryValueType requestedValueType)
{
	if (requestedValueType == KxREG_VALUE_ANY || requestedValueType == KxREG_VALUE_NONE)
	{
		return true;
	}
	else
	{
		switch (realType)
		{
			case REG_BINARY:
			{
				return requestedValueType == KxREG_VALUE_BINARY;
			}
			case REG_SZ:
			case REG_EXPAND_SZ:
			case REG_LINK:
			{
				return requestedValueType == KxREG_VALUE_SZ || requestedValueType == KxREG_VALUE_EXPAND_SZ || requestedValueType == KxREG_VALUE_LINK;
			}
			case REG_MULTI_SZ:
			{
				return requestedValueType == KxREG_VALUE_MULTI_SZ;
			}
			case REG_DWORD:
			case REG_DWORD_BIG_ENDIAN:
			{
				return requestedValueType == KxREG_VALUE_DWORD || requestedValueType == KxREG_VALUE_DWORD_BE;
			}
			case REG_QWORD:
			{
				return requestedValueType == KxREG_VALUE_QWORD || requestedValueType == KxREG_VALUE_QWORD_BE;
			}
		};
		return false;
	}
}

DWORD64 KxRegistry::GetQuota()
{
	DWORD allowed = 0;
	DWORD used = 0;
	DWORD64 ret = 0;
	if (GetSystemRegistryQuota(&allowed, &used))
	{
		ret = KxUtility::MakeInt<DWORD64>(allowed, used);
	}
	return ret;
}

wxAny KxRegistry::GetValueRet(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, KxRegistryValueType& requestedValueType, KxRegistryNode regNode, bool autoExpand)
{
	DWORD flags = RRF_RT_ANY;
	if (autoExpand != true)
	{
		flags |= RRF_NOEXPAND;
	}
	DWORD valueType = 0;
	DWORD bufferSize = 0;
	KxRegistryKey mainKeyhandle(mainKey, regNode, KEY_READ);
	DWORD ret = RegGetValueW(mainKeyhandle, subKey, valueName, flags, &valueType, NULL, &bufferSize);

	wxAny retData;
	if (ret == ERROR_SUCCESS && bufferSize != 0)
	{
		void* data = new byte[bufferSize];
		ret = RegGetValueW(mainKeyhandle, subKey, valueName, flags, &valueType, data, &bufferSize);
		if (ret == ERROR_SUCCESS && IsTypesConform(valueType, requestedValueType))
		{
			KxRegistryValueType oldRequestedValueType = requestedValueType;
			requestedValueType = (KxRegistryValueType)valueType;
			switch (valueType)
			{
				case REG_NONE:
				case REG_BINARY:
				{
					retData = wxAny(wxString::From8BitData((const char*)data, bufferSize));
					break;
				}
				case REG_SZ:
				case REG_LINK:
				case REG_EXPAND_SZ:
				{
					retData = wxAny(wxString((LPCTSTR)data));
					break;
				}
				case REG_DWORD:
				{
					retData = wxAny(*(DWORD*)data);
					break;
				}

				case REG_DWORD_BIG_ENDIAN:
				{
					retData = wxAny(_byteswap_ulong(*(DWORD*)data));
					break;
				}

				case REG_QWORD:
				{
					if (oldRequestedValueType == KxREG_VALUE_QWORD_BE)
					{
						requestedValueType = KxREG_VALUE_QWORD_BE;
						retData = wxAny(_byteswap_uint64(*(DWORD64*)data));
					}
					else
					{
						retData = wxAny(*(DWORD64*)data);
					}
					break;
				}

				case REG_MULTI_SZ:
				{
					DWORD readed = 0;
					DWORD prevRead = 0;
					LPCWSTR value = NULL;

					KxStringVector list;
					while (true)
					{
						value = (LPCWSTR)data+readed;
						if (value != NULL)
						{
							size_t valueLength = wcslen(value);
							DWORD currentLength = valueLength * sizeof(WCHAR)-sizeof(WCHAR);
							readed = readed + currentLength;

							if (valueLength != 0)
							{
								list.push_back(value);
							}
						}
						else
						{
							break;
						}

						if (readed <= prevRead)
						{
							break;
						}
						else
						{
							prevRead = readed;
						}
					}

					retData = wxAny(list);
					break;
				}
			};
		}
		delete[] data;
	}
	return retData;
}
KxRegistryValueType KxRegistry::GetValueType(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, KxRegistryNode regNode)
{
	DWORD valueType = 0;
	KxRegistryKey mainKeyHandle(mainKey, regNode, KEY_READ);
	DWORD ret = RegGetValueW(mainKeyHandle, subKey, valueName, RRF_RT_ANY|RRF_NOEXPAND, &valueType, NULL, NULL);

	if (ret == ERROR_SUCCESS)
	{
		return (KxRegistryValueType)valueType;
	}
	return KxRegistryValueType::KxREG_VALUE_NONE;
}
bool KxRegistry::SetValue(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, wxAny data, KxRegistryValueType nValueType, KxRegistryNode regNode)
{
	KxRegistryKey mainKeyHandle(mainKey, regNode, KEY_WRITE);

	bool ret = false;
	switch (nValueType)
	{
		case KxREG_VALUE_BINARY:
		{
			auto value = data.As<wxMemoryBuffer>();
			ret = RegSetKeyValueW(mainKeyHandle, subKey, valueName, nValueType, value.GetData(), value.GetDataLen());
			break;
		}
		case KxREG_VALUE_SZ:
		case KxREG_VALUE_LINK:
		case KxREG_VALUE_EXPAND_SZ:
		{
			wxString value = data.As<wxString>();
			ret = RegSetKeyValueW(mainKeyHandle, subKey, valueName, nValueType, value.wc_str(), value.Length() * sizeof(WCHAR) + sizeof(WCHAR));
			break;
		}
		case KxREG_VALUE_DWORD:
		case KxREG_VALUE_DWORD_BE:
		{
			DWORD value = data.As<DWORD>();
			ret = RegSetKeyValueW(mainKeyHandle, subKey, valueName, nValueType, &value, sizeof(DWORD));
			break;
		}
		case KxREG_VALUE_QWORD:
		case KxREG_VALUE_QWORD_BE:
		{
			DWORD64 nData = data.As<DWORD64>();
			if (nValueType == KxREG_VALUE_QWORD_BE)
			{
				nValueType = KxREG_VALUE_QWORD;
				nData = _byteswap_uint64(nData);
			}
			ret = RegSetKeyValueW(mainKeyHandle, subKey, valueName, nValueType, &nData, sizeof(DWORD64));
			break;
		}
		case KxREG_VALUE_MULTI_SZ:
		{
			auto list = data.As<KxStringVector>();
			size_t bufferSize = 1;
			for (size_t i = 0; i < list.size(); i++)
			{
				bufferSize += list[i].Length()*sizeof(WCHAR)+sizeof(WCHAR);
			}

			std::vector<uint8_t> dataBuffer;
			dataBuffer.resize(bufferSize);
			size_t written = 0;
			for (size_t i = 0; i < list.size(); i++)
			{
				wxString& value = list[i];
				size_t length = value.Length() * sizeof(WCHAR) + sizeof(WCHAR);

				memcpy(dataBuffer.data() + written, value.wc_str(), length);
				written += length;
			}
			memset(dataBuffer.data() + written, 0, 1);

			ret = RegSetKeyValueW(mainKeyHandle, subKey, valueName, nValueType, dataBuffer.data(), bufferSize);
			break;
		}
	};
	return ret;
}
bool KxRegistry::CreateKey(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode)
{
	return RegCreateKeyExW(KxRegistryKey(mainKey, regNode, KEY_WRITE), subKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, KxRegistryKey(), NULL) == ERROR_SUCCESS;
}

bool KxRegistry::RemoveKey(KxRegistryHKey mainKey, const wxString& subKey, bool recurse, KxRegistryNode regNode)
{
	BOOL ret = FALSE;
	KxRegistryKey keyHandle(mainKey, regNode, KEY_WRITE);
	if (recurse)
	{
		ret = RegDeleteTreeW(keyHandle, subKey);
	}
	else
	{
		ret = RegDeleteKeyExW(keyHandle, subKey, regNode, NULL);
	}
	return ret == ERROR_SUCCESS;
}
bool KxRegistry::RemoveValue(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, KxRegistryNode regNode)
{
	return RegDeleteKeyValueW(KxRegistryKey(mainKey, regNode, KEY_WRITE), subKey, valueName) == ERROR_SUCCESS;
}

bool KxRegistry::IsKeyExist(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode)
{
	return KxRegistryKey(mainKey, subKey, regNode, KEY_QUERY_VALUE).GetStatus() == ERROR_SUCCESS;
}
bool KxRegistry::IsValueExist(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, KxRegistryNode regNode)
{
	return RegQueryValueExW(KxRegistryKey(mainKey, subKey, regNode, KEY_QUERY_VALUE), valueName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS;
}

KxStringVector KxRegistry::GetKeyNames(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode)
{
	KxRegistryKey mainKeyHandle(mainKey, subKey, regNode, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS);
	KxStringVector list;

	DWORD keyCount = 0;
	DWORD retQuery = RegQueryInfoKeyW(mainKeyHandle, NULL, NULL, NULL, &keyCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if (retQuery == ERROR_SUCCESS && keyCount != 0)
	{
		DWORD index = 0;
		list.reserve(keyCount);
		for (DWORD i = 0; i < keyCount; i++)
		{
			DWORD keyNameLength = 0;
			retQuery = RegQueryInfoKeyW(mainKeyHandle, NULL, NULL, NULL, NULL, &keyNameLength, NULL, NULL, NULL, NULL, NULL, NULL);
			if (retQuery == ERROR_SUCCESS)
			{
				keyNameLength += sizeof(WCHAR);
				wxString keyName;
				RegEnumKeyExW(mainKeyHandle, index, wxStringBuffer(keyName, keyNameLength), &keyNameLength, NULL, NULL, NULL, NULL);
				if (keyName.IsEmpty() != true)
				{
					list.push_back(keyName);
				}
				index++;
			}
		}
	}
	return list;
}
KxStringVector KxRegistry::GetValueNames(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode)
{
	KxRegistryKey mainKeyHandle(mainKey, subKey, regNode, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS);
	KxStringVector list;

	DWORD valuesCount = 0;
	DWORD retQuery = RegQueryInfoKeyW(mainKeyHandle, NULL, NULL, NULL, &valuesCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if (retQuery == ERROR_SUCCESS && valuesCount != 0)
	{
		DWORD index = 0;
		list.reserve(valuesCount);
		for (DWORD i = 0; i < valuesCount; i++)
		{
			DWORD valueNameLength = 0;
			retQuery = RegQueryInfoKeyW(mainKeyHandle, NULL, NULL, NULL, NULL, &valueNameLength, NULL, NULL, NULL, NULL, NULL, NULL);
			if (retQuery == ERROR_SUCCESS)
			{
				valueNameLength += sizeof(WCHAR);
				wxString valueName;
				RegEnumValueW(mainKeyHandle, index, wxStringBuffer(valueName, valueNameLength), &valueNameLength, NULL, NULL, NULL, NULL);
				if (valueName.IsEmpty() != true)
				{
					list.push_back(valueName);
				}
				index++;
			}
		}
	}
	return list;
}

//////////////////////////////////////////////////////////////////////////
void KxRegistryKey::OpenKey(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode, DWORD access)
{
	static const HKEY mainKeys[] =
	{
		HKEY_CLASSES_ROOT,
		HKEY_CURRENT_USER,
		HKEY_LOCAL_MACHINE,
		HKEY_USERS,
		HKEY_CURRENT_CONFIG,
		HKEY_PERFORMANCE_DATA,
		HKEY_PERFORMANCE_TEXT,
		HKEY_PERFORMANCE_NLSTEXT
	};

	if (mainKey >= 0 && mainKey < KxRegistryHKey::KxREG_HKEY_MAX && m_Handle == NULL)
	{
		LPCWSTR subKeyString = subKey.IsEmpty() ? NULL : subKey.wc_str();
		m_Status = ::RegOpenKeyExW(mainKeys[mainKey], subKeyString, 0, access|regNode, &m_Handle);
	}
}

KxRegistryKey::KxRegistryKey()
{
}
KxRegistryKey::KxRegistryKey(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode, DWORD access)
{
	OpenKey(mainKey, subKey, regNode, access);
}
KxRegistryKey::KxRegistryKey(KxRegistryHKey mainKey, KxRegistryNode regNode, DWORD access)
{
	OpenKey(mainKey, wxEmptyString, regNode, access);
}
KxRegistryKey::~KxRegistryKey()
{
	if (m_Handle != NULL)
	{
		m_Status = ::RegCloseKey(m_Handle);
	}
}
