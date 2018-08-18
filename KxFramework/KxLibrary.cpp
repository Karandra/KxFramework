#include "KxStdAfx.h"
#include "KxFramework/KxLibrary.h"
#include "KxFramework/KxSystem.h"
#include "KxFramework/KxSystemAPI.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxIncludeWindows.h"
#include <PsAPI.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")
#define LDR_IS_DATAFILE(handle)			(((ULONG_PTR)(handle)) & (ULONG_PTR)1)
#define LDR_IS_IMAGEMAPPING(handle)		(((ULONG_PTR)(handle)) & (ULONG_PTR)2)
#define LDR_IS_RESOURCE(handle)			(LDR_IS_IMAGEMAPPING(handle) || LDR_IS_DATAFILE(handle))

const wxSize KxLibrary::DefaultIconSize(0, 0);
const WORD KxLibrary::DefaultLocaleID = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

BOOL CALLBACK KxLibrary::EnumResourcesProc(HMODULE moduleHandle, LPCTSTR resType, LPTSTR resName, LONG_PTR lParam)
{
	CallbackInfo* info = (CallbackInfo*)lParam;
	KxAnyVector* list = reinterpret_cast<KxAnyVector*>(info->Data);

	if (IS_INTRESOURCE(resName))
	{
		list->push_back((size_t)resName);
	}
	else
	{
		wxString value(resName);
		if (!value.IsEmpty())
		{
			list->push_back(value);
		}
	}
	return TRUE;
}
BOOL CALLBACK KxLibrary::EnumResourceTypesProc(HMODULE moduleHandle, LPTSTR resType, LONG_PTR lParam)
{
	CallbackInfo* info = (CallbackInfo*)lParam;
	KxAnyVector* list = reinterpret_cast<KxAnyVector*>(info->Data);

	if (IS_INTRESOURCE(resType))
	{
		list->push_back((size_t)resType);
	}
	else
	{
		wxString value(resType);
		if (value.IsEmpty() != true)
		{
			list->push_back(value);
		}
	}
	return TRUE;
}
BOOL CALLBACK KxLibrary::EnumResourceLanguagesProc(HMODULE moduleHandle, LPCTSTR resType, LPTSTR resName, WORD langID, LONG_PTR lParam)
{
	CallbackInfo* info = (CallbackInfo*)lParam;
	KxAnyVector* list = reinterpret_cast<KxAnyVector*>(info->Data);

	list->push_back(langID);
	return TRUE;
}

WORD KxLibrary::GetLangID(BYTE primaryID, BYTE secondaryID)
{
	return MAKELANGID(primaryID, secondaryID);
}
WORD KxLibrary::GetLangID(WORD localeID)
{
	// PRIMARYLANGID, SUBLANGID
	return LANGIDFROMLCID(localeID);
}
LPCWSTR KxLibrary::GetNameOrID(const wxString& name)
{
	ULONG id = 0;
	LPCWSTR idString = name.wc_str();
	if (name.ToULong(&id))
	{
		idString = MAKEINTRESOURCEW(id);
	}
	return idString;
}
HRSRC KxLibrary::GetResourceHandle(const wxString& type, const wxString& name, WORD localeID) const
{
	return FindResourceExW(m_Handle, GetNameOrID(type), GetNameOrID(name), GetLangID(localeID));
}
HGLOBAL KxLibrary::LoadResource(HRSRC hResHandle) const
{
	return ::LoadResource(m_Handle, hResHandle);
}
wxMemoryBuffer KxLibrary::GetResource(HRSRC hResHandle) const
{
	if (hResHandle)
	{
		HGLOBAL resDataHandle = LoadResource(hResHandle);
		if (resDataHandle != NULL)
		{
			void* resData = LockResource(resDataHandle);
			DWORD resSize = SizeofResource(m_Handle, hResHandle);
			if (resData && resSize != 0)
			{
				wxMemoryBuffer buffer(resSize);
				buffer.AppendData(resData, resSize);
				return buffer;
			}
		}
	}
	return wxMemoryBuffer();
}

bool KxLibrary::SetSearchFolder(const wxString& path)
{
	if (KxSystemAPI::SetDllDirectoryW)
	{
		if (path.IsEmpty())
		{
			return KxSystemAPI::SetDllDirectoryW(NULL);
		}
		return KxSystemAPI::SetDllDirectoryW(path);
	}
	return false;
}
const void* KxLibrary::AddSearchFolder(const wxString& path)
{
	if (KxSystemAPI::AddDllDirectory)
	{
		return reinterpret_cast<const void*>(KxSystemAPI::AddDllDirectory(path));
	}
	return NULL;
}
bool KxLibrary::RemoveSearchFolder(const void* pathCookie)
{
	if (KxSystemAPI::RemoveDllDirectory)
	{
		return KxSystemAPI::RemoveDllDirectory(reinterpret_cast<DLL_DIRECTORY_COOKIE>(const_cast<void*>(pathCookie)));
	}
	return false;
}

KxLibrary::KxLibrary()
	:KxIOwnedSimple(false)
{
}
KxLibrary::KxLibrary(HMODULE hLibraryHandle)
	:KxIOwnedSimple(false)
{
	Load(hLibraryHandle);
}
KxLibrary::KxLibrary(const wxString& libraryPath, DWORD flags)
	:KxIOwnedSimple(true)
{
	Load(libraryPath, flags);
}
KxLibrary::~KxLibrary()
{
	Unload();
}

bool KxLibrary::Load(HMODULE hLibraryHandle)
{
	if (!IsOK())
	{
		m_Handle = hLibraryHandle;
		m_LoadFlags = 0;
		m_FilePath = GetFileName();

		Disown();
		return IsOK();
	}
	return false;
}
bool KxLibrary::Load(const wxString& libraryPath, DWORD flags)
{
	if (!IsOK())
	{
		m_FilePath = libraryPath;
		m_LoadFlags = flags;
		m_Handle = ::LoadLibraryExW(libraryPath, NULL, flags);

		TakeOwnership();
		return IsOK();
	}
	return false;
}
void KxLibrary::Unload()
{
	if (IsOwned() && IsOK())
	{
		::FreeLibrary(m_Handle);
	}
	Disown();

	if (IsUpdatingResources())
	{
		EndUpdateResource(true);
	}
}

// Version info
wxString KxLibrary::NumberVersionToString(DWORD mostSignificant, DWORD leastSignificant)
{
	return wxString::Format("%d.%d.%d.%d", HIWORD(mostSignificant), LOWORD(mostSignificant), HIWORD(leastSignificant), LOWORD(leastSignificant));
}
void KxLibrary::StringVersionToNumber(const wxString& version, DWORD& mostSignificant, DWORD& leastSignificant)
{
	WORD versionArray[4] = {0, 0, 0, 0};
	WCHAR* current = (WCHAR*)version.wc_str();
	for (int i = 0; i < 4; i++)
	{
		versionArray[i] = wcstoul(current, &current, 10);
		current++;
	}

	mostSignificant = MAKELONG(versionArray[1], versionArray[0]);
	leastSignificant = MAKELONG(versionArray[3], versionArray[2]);
}
void KxLibrary::SaveVersionString(const KxLibraryVersionInfo& info, const wxString& queryTemplate, const wxMemoryBuffer& buffer, const wxString& rawFiledName, const wxString& infoFiledName)
{
	wxString query = wxString::Format("%s\\%s", queryTemplate, rawFiledName);
	UINT size = 0;
	LPWSTR stringInfo = NULL;
	if (::VerQueryValueW(buffer.GetData(), query, (void**)&stringInfo, &size) && stringInfo)
	{
		wxString value = info.GetString(rawFiledName);
		if (value.IsEmpty())
		{
			value = info.GetString(infoFiledName);
		}
		memset(stringInfo, 0, sizeof(WCHAR) * size);
		wcsncpy(stringInfo, value, size);
	}
}
void KxLibrary::LoadVersionString(KxLibraryVersionInfo& info, const wxString& queryTemplate, const wxMemoryBuffer& buffer, const wxString& rawFiledName, const wxString& infoFiledName)
{
	wxString query = wxString::Format("%s\\%s", queryTemplate, rawFiledName);
	UINT size = 0;
	LPWSTR stringInfo = NULL;
	if (::VerQueryValueW(buffer.GetData(), query, (void**)&stringInfo, &size) && stringInfo)
	{
		info.SetString(infoFiledName, wxString(stringInfo, size));
	}
}
wxMemoryBuffer KxLibrary::CreateVersionInfoStruct(const wxString& templateString, const KxLibraryVersionInfo& info)
{
	DWORD structSize = ::GetFileVersionInfoSizeW(templateString, NULL);
	if (structSize != 0)
	{
		wxMemoryBuffer buffer(structSize);
		BOOL ret = ::GetFileVersionInfoW(templateString, 0, structSize, buffer.GetData());
		if (ret)
		{
			void* fixedInfo = NULL;
			UINT size = 0;
			ret = ::VerQueryValueW(buffer.GetData(), L"\\", &fixedInfo, &size);
			if (ret != 0 && fixedInfo != NULL)
			{
				VS_FIXEDFILEINFO* fixedVersionInfo = (VS_FIXEDFILEINFO*)fixedInfo;

				StringVersionToNumber(info.GetString("FileVersion"), fixedVersionInfo->dwFileVersionMS, fixedVersionInfo->dwFileVersionLS);
				StringVersionToNumber(info.GetString("ProductVersion"), fixedVersionInfo->dwProductVersionMS, fixedVersionInfo->dwProductVersionLS);

				// Set date
				SYSTEMTIME systemTime = {0};
				::GetLocalTime(&systemTime);
				FILETIME tTime = {0};
				::SystemTimeToFileTime(&systemTime, &tTime);
				fixedVersionInfo->dwFileDateMS = tTime.dwLowDateTime;
				fixedVersionInfo->dwFileDateLS = tTime.dwHighDateTime;
			}

			// Language
			void* langInfo = NULL;
			size = 0;
			ret = ::VerQueryValueW(buffer.GetData(), L"\\VarFileInfo\\Translation", &langInfo, &size);
			if (ret)
			{
				KxLibraryVersionInfo::VerQueryValueLangInfo* langInfo2 = (KxLibraryVersionInfo::VerQueryValueLangInfo*)langInfo;

				wxString queryTemplate = wxString::Format("\\StringFileInfo\\%04x%04x", langInfo2->Language, langInfo2->CodePage);
				for (size_t i = 0; i < KxLibraryVersionInfo::VersionInfoFieldNamesCount; i++)
				{
					SaveVersionString(info, queryTemplate, buffer, KxLibraryVersionInfo::VersionInfoRawFieldNames[i], KxLibraryVersionInfo::VersionInfoFieldNames[i]);
				}
			}

			return buffer;
		}
	}
	return wxMemoryBuffer();
}
KxLibraryVersionInfo KxLibrary::GetVersionInfoFromFile(const wxString& filePath)
{
	KxLibraryVersionInfo info;
	DWORD structSize = ::GetFileVersionInfoSizeW(filePath, NULL);
	if (structSize != 0)
	{
		wxMemoryBuffer buffer(structSize);
		BOOL ret = ::GetFileVersionInfoW(filePath, 0, structSize, buffer.GetData());
		if (ret)
		{
			void* fixedInfo = NULL;
			UINT size = 0;
			ret = ::VerQueryValueW(buffer.GetData(), L"\\", &fixedInfo, &size);
			if (ret != 0 && fixedInfo != NULL)
			{
				info.SetInit();

				VS_FIXEDFILEINFO* fixedVersionInfo = (VS_FIXEDFILEINFO*)fixedInfo;
				info.SetString("FileVersion", NumberVersionToString(fixedVersionInfo->dwFileVersionMS, fixedVersionInfo->dwFileVersionLS));
				info.SetString("ProductVersion", NumberVersionToString(fixedVersionInfo->dwProductVersionMS, fixedVersionInfo->dwProductVersionLS));
				
				// Time
				FILETIME time = {0};
				time.dwHighDateTime = fixedVersionInfo->dwFileDateLS;
				time.dwLowDateTime = fixedVersionInfo->dwFileDateMS;

				SYSTEMTIME systemTime = {0};
				::FileTimeToSystemTime(&time, &systemTime);

				info.SetString("FileDate", wxDateTime().SetFromMSWSysTime(systemTime).FormatISOCombined());
			}

			// Language
			void* langInfo = NULL;
			size = 0;
			ret = ::VerQueryValueW(buffer.GetData(), L"\\VarFileInfo\\Translation", &langInfo, &size);
			if (ret)
			{
				info.SetInit();
				KxLibraryVersionInfo::VerQueryValueLangInfo* langInfo2 = (KxLibraryVersionInfo::VerQueryValueLangInfo*)langInfo;

				wxString queryTemplate = wxString::Format("\\StringFileInfo\\%04x%04x", langInfo2->Language, langInfo2->CodePage);
				for (size_t i = 0; i < KxLibraryVersionInfo::VersionInfoFieldNamesCount; i++)
				{
					LoadVersionString(info, queryTemplate, buffer, KxLibraryVersionInfo::VersionInfoRawFieldNames[i], KxLibraryVersionInfo::VersionInfoFieldNames[i]);
				}
			}
		}
	}
	return info;
}

// Properties
bool KxLibrary::IsOK() const
{
	return m_Handle != NULL;
}
bool KxLibrary::IsDataFile() const
{
	return LDR_IS_DATAFILE(m_Handle);
}
bool KxLibrary::IsImageDataFile() const
{
	return LDR_IS_IMAGEMAPPING(m_Handle);
}
bool KxLibrary::IsResource() const
{
	return LDR_IS_RESOURCE(m_Handle);
}
HMODULE KxLibrary::GetHandle() const
{
	return m_Handle;
}
void KxLibrary::SetHandle(HMODULE hHandle)
{
	m_Handle = hHandle;
}
wxString KxLibrary::GetFileName() const
{
	if (IsResource())
	{
		return m_FilePath;
	}
	else
	{
		wxString path;
		if (GetModuleFileNameExW(GetCurrentProcess(), m_Handle, wxStringBuffer(path, INT16_MAX), INT16_MAX))
		{
			return path;
		}
		return m_FilePath;
	}
}
DWORD KxLibrary::GetLoadFlags() const
{
	return m_LoadFlags;
}

// Resources
bool KxLibrary::BeginUpdateResource()
{
	if (!IsUpdatingResources())
	{
		FreeLibrary(GetHandle());
		SetHandle(NULL);

		m_UpdateHandle = BeginUpdateResourceW(GetFileName(), FALSE);
		return m_UpdateHandle != NULL;
	}
	return false;
}
bool KxLibrary::EndUpdateResource(bool noReopen)
{
	if (IsUpdatingResources())
	{
		bool value = EndUpdateResourceW(m_UpdateHandle, FALSE);
		m_UpdateHandle = NULL;

		if (!noReopen)
		{
			HMODULE libraryHandle = LoadLibraryExW(GetFileName(), NULL, GetLoadFlags());
			SetHandle(libraryHandle);
			value = value && libraryHandle != NULL;
		}
		return value;
	}
	return false;
}
bool KxLibrary::UpdateResource(const wxString& type, const wxString& name, const wxMemoryBuffer& data, WORD localeID, bool updateNow)
{
	if (updateNow)
	{
		BeginUpdateResource();
	}

	if (IsUpdatingResources())
	{
		WORD langID = KxLibrary::GetLangID(localeID);
		bool value = UpdateResourceW(m_UpdateHandle, KxLibrary::GetNameOrID(type.Upper()), KxLibrary::GetNameOrID(name.Upper()), langID, data.GetData(), data.GetDataLen());
		if (updateNow)
		{
			EndUpdateResource();
		}
		return value;
	}
	return false;
}
bool KxLibrary::IsUpdatingResources() const
{
	return m_UpdateHandle != NULL;
}

KxIntVector KxLibrary::EnumResourceLanguages(const wxString& type, const wxString& name) const
{
	KxIntVector list;

	CallbackInfo info(const_cast<KxLibrary*>(this), &list);
	EnumResourceLanguagesW(m_Handle, GetNameOrID(type), GetNameOrID(name), (ENUMRESLANGPROCW)EnumResourceLanguagesProc, (LONG_PTR)&info);
	return list;
}
KxAnyVector KxLibrary::EnumResourceTypes(WORD localeID) const
{
	KxAnyVector list;

	CallbackInfo info(const_cast<KxLibrary*>(this), &list);
	EnumResourceTypesExW(m_Handle, EnumResourceTypesProc, (LONG_PTR)&info, 0, GetLangID(localeID));
	return list;
}
KxAnyVector KxLibrary::EnumResources(const wxString& type, WORD localeID) const
{
	KxAnyVector list;

	CallbackInfo info(const_cast<KxLibrary*>(this), &list);
	EnumResourceNamesExW(m_Handle, GetNameOrID(type), EnumResourcesProc, (LONG_PTR)&info, 0, GetLangID(localeID));
	return list;
}
wxMemoryBuffer KxLibrary::GetResource(const wxString& type, const wxString& name, WORD localeID) const
{
	return GetResource(GetResourceHandle(type, name, localeID));
}
wxBitmap KxLibrary::GetBitmap(const wxString& name, WORD localeID) const
{
	return LoadGDIImage<wxBitmap>(name, ResIDToName(RT_BITMAP), IMAGE_BITMAP, DefaultIconSize, localeID);
}
wxIcon KxLibrary::GetIcon(const wxString& name, wxSize size, WORD localeID) const
{
	return LoadGDIImage<wxIcon>(name, ResIDToName(RT_GROUP_ICON), IMAGE_ICON, size, localeID);
}
wxIcon KxLibrary::GetIcon(const wxString& name, size_t index, WORD localeID) const
{
	wxMemoryBuffer groupBuffer = GetResource(GetResourceHandle(ResIDToName(RT_GROUP_ICON), name, localeID));
	if (!groupBuffer.IsEmpty())
	{
		IconGroupDirectory* iconGroup = (IconGroupDirectory*)groupBuffer.GetData();
		WORD iconCount = iconGroup->idCount;
		if (index >= iconCount)
		{
			index = iconCount - 1;
		}

		IconGroupEntry* iconInfo = &iconGroup->idEntries[index];
		WORD imageID = iconInfo->id;

		wxMemoryBuffer iconBuffer = GetResource(GetResourceHandle(ResIDToName(RT_ICON), ResIDToName(imageID), localeID));
		if (!iconBuffer.IsEmpty())
		{
			int width = 0;
			int height = 0;

			// I don't remember what '0x00030000' is.
			HICON iconHandle = CreateIconFromResourceEx((PBYTE)iconBuffer.GetData(), iconBuffer.GetDataLen(), TRUE, 0x00030000, width, height, LR_DEFAULTCOLOR);
			if (iconHandle != NULL)
			{
				wxIcon image;
				image.SetDepth(iconInfo->wBitCount);
				image.SetHandle(iconHandle);
				return image;
			}
		}
	}
	return wxNullIcon;
}
size_t KxLibrary::GetIconCount(const wxString& name, WORD localeID) const
{
	wxMemoryBuffer groupBuffer = GetResource(GetResourceHandle(ResIDToName(RT_GROUP_ICON), name));
	if (!groupBuffer.IsEmpty())
	{
		IconGroupDirectory* tIconGroup = (IconGroupDirectory*)groupBuffer.GetData();
		return tIconGroup->idCount;
	}
	return 0;
}
wxCursor KxLibrary::GetCursor(const wxString& name, WORD localeID) const
{
	return LoadGDIImage<wxCursor>(name, ResIDToName(RT_CURSOR), IMAGE_CURSOR, DefaultIconSize, localeID);
}
wxString KxLibrary::GetString(const wxString& name, WORD localeID) const
{
	if (localeID == 0)
	{
		// LoadString can not load strings specified by locale ID
		long stringID = 0;
		name.ToLong(&stringID);

		LPCWSTR string = NULL;
		int length = LoadStringW(m_Handle, stringID, (LPWSTR)&string, 0);
		if (length > 0)
		{
			return wxString(string, length);
		}
	}
	else
	{
		// http://forum.sources.ru/index.php?showtopic=375357
		long stringID = 0;
		if (name.ToLong(&stringID))
		{
			wxMemoryBuffer data = GetResource(ResIDToName(RT_STRING), ResIDToName(MAKEINTRESOURCEW(stringID / 16 + 1)), localeID);
			if (!data.IsEmpty())
			{
				stringID = stringID % 16;
				DWORD tableSize = data.GetDataLen() / sizeof(WCHAR);
				DWORD offset = 0;
				DWORD index = 0;

				const WCHAR* stringData = (const WCHAR*)data.GetData();
				while (offset < tableSize)
				{
					if (index == (DWORD)stringID)
					{
						DWORD stringLength = stringData[offset];
						if (stringLength > 0)
						{
							return wxString(&(stringData[offset + 1]), stringLength);
						}
						break;
					}
					offset += stringData[offset] + 1;
					index++;
				}
			}
		}
	}
	return wxEmptyString;
}
wxString KxLibrary::FormatMessage(DWORD messageID, WORD localeID) const
{
	return KxSystem::FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_HMODULE, m_Handle, messageID, GetLangID(localeID));
}

bool KxLibrary::IsResourceExist(const wxString& type, const wxString& name, WORD localeID)
{
	return GetResourceHandle(type, name, localeID) != NULL;
}
bool KxLibrary::RemoveResource(const wxString& type, const wxString& name, WORD localeID, bool updateNow)
{
	KxLibraryUpdateLocker locker(this);
	if (locker.IsOK())
	{
		locker.UpdateResource(type, name, wxMemoryBuffer(), localeID);
	}
	return locker.IsSuccess();
	//return UpdateResource(type, name, wxMemoryBuffer(NULL, 0, ALLOC_NONE, true), localeID, updateNow);
}
bool KxLibrary::UpdateResource(const wxString& type, const wxString& name, const wxMemoryBuffer& data, bool overwrite, WORD localeID, bool updateNow)
{
	if (overwrite || !IsResourceExist(type, name, localeID))
	{
		KxLibraryUpdateLocker locker(this);
		if (locker.IsOK())
		{
			locker.UpdateResource(type, name, data, localeID);
		}
		return locker.IsSuccess();
	}
	return false;
}

// Functions
KxStringVector KxLibrary::EnumFunctions() const
{
	KxStringVector functionNames;

	HANDLE fileHandle = ::CreateFileW(GetFileName(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		HANDLE fileMapping = ::CreateFileMappingW(fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
		if (fileMapping != NULL)
		{
			LPVOID fileBase = ::MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
			if (fileBase != NULL)
			{
				PIMAGE_DOS_HEADER img_DOS_Header = (PIMAGE_DOS_HEADER)fileBase;
				PIMAGE_NT_HEADERS img_NT_Header = (PIMAGE_NT_HEADERS)((size_t)img_DOS_Header + (size_t)img_DOS_Header->e_lfanew);
				if (!(::IsBadReadPtr(img_NT_Header, sizeof(IMAGE_NT_HEADERS)) || img_NT_Header->Signature != IMAGE_NT_SIGNATURE))
				{
					#pragma warning(suppress: 4312)
					PIMAGE_EXPORT_DIRECTORY img_Export_Dir = (PIMAGE_EXPORT_DIRECTORY)img_NT_Header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
					if (img_Export_Dir)
					{
						img_Export_Dir = (PIMAGE_EXPORT_DIRECTORY)::ImageRvaToVa(img_NT_Header, img_DOS_Header, (size_t)img_Export_Dir, 0);

						#pragma warning(suppress: 4312)
						DWORD** namesArray = (DWORD**)img_Export_Dir->AddressOfNames;
						namesArray = (DWORD**)::ImageRvaToVa(img_NT_Header, img_DOS_Header, (size_t)namesArray, 0);
						if (namesArray != NULL)
						{
							DWORD functionsCount = img_Export_Dir->NumberOfNames;
							functionNames.reserve(functionsCount);
							for (DWORD i = 0; i < functionsCount; i++)
							{
								#pragma warning(suppress: 4302)
								#pragma warning(suppress: 4311)
								const char* name = (const char*)ImageRvaToVa(img_NT_Header, img_DOS_Header, (DWORD)(*namesArray), 0);
								functionNames.push_back(name);
								namesArray++;
							}
						}
					}
				}
				UnmapViewOfFile(fileBase);
			}
			CloseHandle(fileMapping);
		}
		CloseHandle(fileHandle);
	}

	return functionNames;
}
void* KxLibrary::GetProcAddress(const wxString& name) const
{
	return ::GetProcAddress(m_Handle, name);
}
void* KxLibrary::GetProcAddress(WORD nOrdinal) const
{
	return ::GetProcAddress(m_Handle, MAKEINTRESOURCEA(nOrdinal));
}
bool KxLibrary::IsFunctionExist(const wxString& name) const
{
	return GetProcAddress(name);
}
bool KxLibrary::IsFunctionExist(WORD nOrdinal) const
{
	return GetProcAddress(nOrdinal);
}
#if defined RtCFunction
RtCFunction* KxLibrary::Function(const wxString& name, lua_State* L, const Lua::IntegerArray& tArgTypes, LClassID nRetTypeID, RtCFunctionABI nABI)
{
	void* pFunction = GetProcAddress(name);
	if (pFunction)
	{
		RtCFunction* pCFunction = new RtCFunction(pFunction, tArgTypes, nRetTypeID, nABI);
		pCFunction->SetName(wxString::Format("%s::%s", wxFileName(GetFileName()).GetName(), name));
		return pCFunction;
	}
	return NULL;
}
RtCFunction* KxLibrary::Function(WORD nOrdinal, lua_State* L, const Lua::IntegerArray& tArgTypes, LClassID nRetTypeID, RtCFunctionABI nABI)
{
	void* pFunction = GetProcAddress(nOrdinal);
	if (pFunction)
	{
		RtCFunction* pCFunction = new RtCFunction(pFunction, tArgTypes, nRetTypeID, nABI);
		pCFunction->SetName(wxString::Format("%s!0x%p", wxFileName(GetFileName()).GetFullName(), pFunction));
		return pCFunction;
	}
	return NULL;
}
#endif

//////////////////////////////////////////////////////////////////////////
void KxLibraryUpdateLocker::Unlock()
{
	::FreeLibrary(m_Library->GetHandle());
	m_Library->SetHandle(NULL);
}
void KxLibraryUpdateLocker::Lock()
{
	HMODULE libraryHandle = ::LoadLibraryExW(m_LibarryFullPath, NULL, m_Library->GetLoadFlags());
	m_Library->SetHandle(libraryHandle);
}

KxLibraryUpdateLocker::KxLibraryUpdateLocker(KxLibrary* library)
	:m_Library(library), m_LibarryFullPath(library->GetFileName())
{
	m_UpdateHandle = ::BeginUpdateResourceW(m_LibarryFullPath, FALSE);
}
KxLibraryUpdateLocker::~KxLibraryUpdateLocker()
{
}

void KxLibraryUpdateLocker::UpdateResource(const wxString& type, const wxString& name, const wxMemoryBuffer& data, WORD localeID)
{
	if (IsOK())
	{
		Unlock();

		WORD langID = KxLibrary::GetLangID(localeID);
		if (::UpdateResourceW(GetHandle(), KxLibrary::GetNameOrID(type.Upper()), KxLibrary::GetNameOrID(name.Upper()), langID, data.GetData(), data.GetDataLen()))
		{
			m_Success = ::EndUpdateResourceW(m_UpdateHandle, FALSE);
		}
		Lock();
		return;
	}
	m_Success = false;
}

//////////////////////////////////////////////////////////////////////////
const LPCWSTR KxLibraryVersionInfo::VersionInfoRawFieldNames[] =
{
	L"FileVersion",
	L"ProductVersion",
	L"FileDescription",
	L"InternalName",
	L"OriginalFilename",
	L"CompanyName",
	L"ProductName",
	L"LegalCopyright",
	L"LegalTrademarks",
	L"PrivateBuild",
	L"SpecialBuild",
};
const LPCWSTR KxLibraryVersionInfo::VersionInfoFieldNames[] =
{
	L"FileVersionString",
	L"ProductVersionString",
	L"FileDescription",
	L"InternalName",
	L"OriginalFilename",
	L"CompanyName",
	L"ProductName",
	L"LegalCopyright",
	L"LegalTrademarks",
	L"PrivateBuild",
	L"SpecialBuild",
};
const size_t KxLibraryVersionInfo::VersionInfoFieldNamesCount = ARRAYSIZE(VersionInfoFieldNames);
