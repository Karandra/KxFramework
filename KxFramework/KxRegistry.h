/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

enum KxRegistryHKey
{
	KxREG_HKEY_CLASSES_ROOT,
	KxREG_HKEY_CURRENT_USER,
	KxREG_HKEY_LOCAL_MACHINE,
	KxREG_HKEY_USERS,
	KxREG_HKEY_CURRENT_CONFIG,
	KxREG_HKEY_PERFORMANCE_DATA,
	KxREG_HKEY_PERFORMANCE_TEXT,
	KxREG_HKEY_PERFORMANCE_NLSTEXT,

	KxREG_HKEY_MAX
};
enum KxRegistryAccess
{
	KxREG_ACCESS_READ,
	KxREG_ACCESS_WRITE,
	KxREG_ACCESS_ENUMERATE,
	KxREG_ACCESS_ALL
};
enum KxRegistryValueType
{
	KxREG_VALUE_ANY = -1,
	KxREG_VALUE_NONE = REG_NONE,
	KxREG_VALUE_SZ = REG_SZ,
	KxREG_VALUE_EXPAND_SZ = REG_EXPAND_SZ,
	KxREG_VALUE_MULTI_SZ = REG_MULTI_SZ,
	KxREG_VALUE_BINARY = REG_BINARY,
	KxREG_VALUE_DWORD = REG_DWORD,
	KxREG_VALUE_DWORD_LE = REG_DWORD_LITTLE_ENDIAN,
	KxREG_VALUE_DWORD_BE = REG_DWORD_BIG_ENDIAN,
	KxREG_VALUE_QWORD = REG_QWORD,
	KxREG_VALUE_QWORD_LE = REG_QWORD_LITTLE_ENDIAN,
	KxREG_VALUE_QWORD_BE,
	KxREG_VALUE_LINK = REG_LINK,
	KxREG_VALUE_RESOURCE_LIST = REG_RESOURCE_LIST,
	KxREG_VALUE_FULL_RESOURCE_DESCRIPTOR = REG_FULL_RESOURCE_DESCRIPTOR,
	KxREG_VALUE_RESOURCE_REQUIREMENTS_LIST = REG_RESOURCE_REQUIREMENTS_LIST
};
enum KxRegistryNode
{
	KxREG_NODE_SYS = 0,
	KxREG_NODE_32 = KEY_WOW64_32KEY,
	KxREG_NODE_64 = KEY_WOW64_64KEY
};

class KX_API KxRegistryKey;
class KX_API KxRegistry
{
	private:
		static bool IsTypesConform(DWORD realType, KxRegistryValueType requestedValueType);

	public:
		static DWORD64 GetQuota();

		static wxAny GetValue(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, KxRegistryValueType requestedValueType, KxRegistryNode regNode = KxREG_NODE_SYS, bool autoExpand = false)
		{
			KxRegistryValueType actualType = requestedValueType;
			return GetValueRet(mainKey, subKey, valueName, actualType, regNode, autoExpand);
		}
		static wxAny GetValueRet(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, KxRegistryValueType& requestedValueType, KxRegistryNode regNode = KxREG_NODE_SYS, bool autoExpand = false);
		static KxRegistryValueType GetValueType(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, KxRegistryNode regNode = KxREG_NODE_SYS);
		static bool SetValue(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, const wxAny& data, KxRegistryValueType nValueType = KxREG_VALUE_SZ, KxRegistryNode regNode = KxREG_NODE_SYS);
		static bool CreateKey(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode = KxREG_NODE_SYS);
		
		static bool RemoveKey(KxRegistryHKey mainKey, const wxString& subKey, bool recurse = false, KxRegistryNode regNode = KxREG_NODE_SYS);
		static bool RemoveValue(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, KxRegistryNode regNode = KxREG_NODE_SYS);
		
		static bool IsKeyExist(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode = KxREG_NODE_SYS);
		static bool IsValueExist(KxRegistryHKey mainKey, const wxString& subKey, const wxString& valueName, KxRegistryNode regNode = KxREG_NODE_SYS);
		
		static KxStringVector GetKeyNames(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode = KxREG_NODE_SYS);
		static KxStringVector GetValueNames(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode = KxREG_NODE_SYS);
};

class KX_API KxRegistryKey
{
	private:
		HKEY m_Handle = nullptr;
		DWORD m_Status = ERROR_INVALID_FUNCTION; // Init to some error code

	private:
		void OpenKey(KxRegistryHKey mainKey, const wxString& subKey = wxEmptyString, KxRegistryNode regNode = KxREG_NODE_SYS, DWORD access = KEY_READ|KEY_WRITE);

	public:
		KxRegistryKey();
		KxRegistryKey(KxRegistryHKey mainKey, const wxString& subKey, KxRegistryNode regNode = KxREG_NODE_SYS, DWORD access = KEY_READ|KEY_WRITE);
		KxRegistryKey(KxRegistryHKey mainKey, KxRegistryNode regNode = KxREG_NODE_SYS, DWORD access = KEY_READ|KEY_WRITE);
		~KxRegistryKey();

	public:
		DWORD GetStatus() const
		{
			return m_Status;
		}
		operator bool() const
		{
			return m_Handle != nullptr && m_Status == ERROR_SUCCESS;
		}
		
		operator HKEY() const
		{
			return m_Handle;
		}
		operator HKEY*()
		{
			return &m_Handle;
		}
		operator HKEY&()
		{
			return m_Handle;
		}
		
		KxRegistryKey& operator=(HKEY keyHandle)
		{
			m_Handle = keyHandle;
			return *this;
		}
};
