#include "KxStdAfx.h"
#include "KxFramework/KxLibrary.h"
#include "KxFramework/KxSystemAPI.h"
#include "KxFramework/KxIncludeWindows.h"
#include <PsAPI.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

namespace Util
{
	static const WORD DefaultLocaleID = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
	static const wxSize DefaultIconSize(0, 0);

	bool IsLibraryDataFile(HMODULE handle)
	{
		return (ULONG_PTR)(handle) & (ULONG_PTR)1;
	}
	bool IsLibraryImageResourse(HMODULE handle)
	{
		return (ULONG_PTR)(handle) & (ULONG_PTR)2;
	}
	bool IsLibraryResource(HMODULE handle)
	{
		return IsLibraryDataFile(handle) || IsLibraryImageResourse(handle);
	}

	WORD GetLangID(BYTE primaryID, BYTE secondaryID)
	{
		return MAKELANGID(primaryID, secondaryID);
	}
	WORD GetLangID(WORD localeID)
	{
		// Useful macros: PRIMARYLANGID, SUBLANGID
		return LANGIDFROMLCID(localeID);
	}
	LPCWSTR GetNameOrID(const wxString& name)
	{
		ULONG id = 0;
		LPCWSTR idString = name.wc_str();
		if (name.ToULong(&id))
		{
			idString = MAKEINTRESOURCEW(id);
		}
		return idString;
	}

	HRSRC GetResourceHandle(HMODULE handle, const wxString& type, const wxString& name, WORD localeID = DefaultLocaleID)
	{
		return ::FindResourceExW(handle, GetNameOrID(type), GetNameOrID(name), GetLangID(localeID));
	}
	KxFramework::UntypedMemorySpan GetResource(HMODULE handle, HRSRC resHandle)
	{
		if (resHandle)
		{
			HGLOBAL resDataHandle = ::LoadResource(handle, resHandle);
			if (resDataHandle != nullptr)
			{
				void* resData = LockResource(resDataHandle);
				DWORD resSize = SizeofResource(handle, resHandle);
				if (resData && resSize != 0)
				{
					return KxFramework::UntypedMemorySpan(resData, resSize);
				}
			}
		}
		return {};
	}

	HANDLE LoadGDIImageAux(HMODULE handle, const wxString& name, const wxString& type, UINT GDIType, wxSize size = DefaultIconSize, WORD localeID = DefaultLocaleID)
	{
		HGLOBAL resDataHandle = ::LoadResource(handle, GetResourceHandle(handle, type, name, localeID));
		if (resDataHandle)
		{
			return LoadImageW(handle, GetNameOrID(name), GDIType, size.GetWidth(), size.GetHeight(), LR_DEFAULTCOLOR);
		}
		return nullptr;
	}
	template<class T> T LoadGDIImage(HMODULE handle, const wxString& name, const wxString& type, UINT GDIType, wxSize size = DefaultIconSize, WORD localeID = DefaultLocaleID)
	{
		HANDLE imageHandle = LoadGDIImageAux(handle, name, type, GDIType, size, localeID);
		if (imageHandle)
		{
			T image;
			image.SetHandle(imageHandle);
			if (size.IsFullySpecified())
			{
				// These functions were removed in wxWidgets 3.1.2
				//image.SetWidth(size.GetWidth());
				//image.SetHeight(size.GetHeight());
			}
			return image;
		}
		return T();
	}
	template<> wxIcon LoadGDIImage<wxIcon>(HMODULE handle, const wxString& name, const wxString& type, UINT GDIType, wxSize size, WORD localeID)
	{
		HANDLE imageHandle = LoadGDIImageAux(handle, name, type, GDIType, size, localeID);
		if (imageHandle)
		{
			wxIcon icon;
			icon.CreateFromHICON(static_cast<HICON>(imageHandle));
			return icon;
		}
		return wxNullIcon;
	}

	#pragma pack(push)
	#pragma pack(2)
	struct IconGroupEntry
	{
		BYTE   bWidth;               // Width, in pixels, of the image
		BYTE   bHeight;              // Height, in pixels, of the image
		BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
		BYTE   bReserved;            // Reserved
		WORD   wPlanes;              // Color Planes
		WORD   wBitCount;            // Bits per pixel
		DWORD  dwBytesInRes;         // how many bytes in this resource?
		WORD   id;                  // the ID
	};
	#pragma pack(pop)

	#pragma pack(push)
	#pragma pack(2)
	struct IconGroupDirectory
	{
		WORD            idReserved;   // Reserved (must be 0)
		WORD            idType;       // Resource type (1 for icons)
		WORD            idCount;      // How many images?
		IconGroupEntry idEntries[1]; // The entries for each image
	};
	#pragma pack(pop)

	struct CallbackInfo
	{
		KxLibrary* self = nullptr;
		void* Data = nullptr;

		CallbackInfo(KxLibrary* self, void* data = nullptr)
			:self(self), Data(data)
		{
		}
	};
	BOOL CALLBACK EnumResourcesProc(HMODULE moduleHandle, LPCTSTR resType, LPTSTR resName, LONG_PTR lParam)
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
	BOOL CALLBACK EnumResourceTypesProc(HMODULE moduleHandle, LPTSTR resType, LONG_PTR lParam)
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
	BOOL CALLBACK EnumResourceLanguagesProc(HMODULE moduleHandle, LPCTSTR resType, LPTSTR resName, WORD langID, LONG_PTR lParam)
	{
		CallbackInfo* info = (CallbackInfo*)lParam;
		KxAnyVector* list = reinterpret_cast<KxAnyVector*>(info->Data);

		list->push_back(langID);
		return TRUE;
	}
}

class KxLibraryUpdateLocker
{
	private:
		HANDLE m_UpdateHandle = nullptr;
		bool m_Success = false;
		KxLibrary* m_Library = nullptr;
		wxString m_LibaryFullPath;

	private:
		void Unlock()
		{
			::FreeLibrary(m_Library->GetHandle());
			m_Library->SetHandle(nullptr);
		}
		void Lock()
		{
			HMODULE libraryHandle = ::LoadLibraryExW(m_LibaryFullPath.wc_str(), nullptr, m_Library->GetLoadFlags());
			m_Library->SetHandle(libraryHandle);
		}

	public:
		KxLibraryUpdateLocker(KxLibrary* library)
			:m_LibaryFullPath(library->GetFileName())
		{
			m_UpdateHandle = ::BeginUpdateResourceW(m_LibaryFullPath.wc_str(), FALSE);
		}

	public:
		void UpdateResource(const wxString& type, const wxString& name, const KxFramework::UntypedMemorySpan& data, WORD localeID)
		{
			if (IsOK())
			{
				Unlock();

				WORD langID = Util::GetLangID(localeID);
				if (::UpdateResourceW(GetHandle(), Util::GetNameOrID(type.Upper()), Util::GetNameOrID(name.Upper()), langID, const_cast<void*>(data.data()), data.size()))
				{
					m_Success = ::EndUpdateResourceW(m_UpdateHandle, FALSE);
				}
				Lock();
				return;
			}
			m_Success = false;
		}
		bool IsOK() const
		{
			return m_UpdateHandle != nullptr && m_Library != nullptr;
		}
		bool IsSuccess() const
		{
			return m_Success;
		}
		HANDLE GetHandle() const
		{
			return m_UpdateHandle;
		}
};

//////////////////////////////////////////////////////////////////////////
const WORD KxLibrary::DefaultLocaleID = Util::DefaultLocaleID;
const wxSize KxLibrary::DefaultIconSize = Util::DefaultIconSize;

bool KxLibrary::SetSearchFolder(const wxString& path)
{
	if (KxSystemAPI::SetDllDirectoryW)
	{
		if (path.IsEmpty())
		{
			return KxSystemAPI::SetDllDirectoryW(nullptr);
		}
		return KxSystemAPI::SetDllDirectoryW(path.wc_str());
	}
	return false;
}
const void* KxLibrary::AddSearchFolder(const wxString& path)
{
	if (KxSystemAPI::AddDllDirectory)
	{
		return reinterpret_cast<const void*>(KxSystemAPI::AddDllDirectory(path.wc_str()));
	}
	return nullptr;
}
bool KxLibrary::RemoveSearchFolder(const void* pathCookie)
{
	if (KxSystemAPI::RemoveDllDirectory)
	{
		return KxSystemAPI::RemoveDllDirectory(reinterpret_cast<DLL_DIRECTORY_COOKIE>(const_cast<void*>(pathCookie)));
	}
	return false;
}

bool KxLibrary::Load(HMODULE libraryHandle)
{
	if (!IsOK())
	{
		m_Handle = libraryHandle;
		m_FilePath = GetFileName();
		
		if (IsOK())
		{
			m_OwnHandle = false;
			m_LoadFlags = 0;
			if (IsDataFile())
			{
				m_LoadFlags |= LOAD_LIBRARY_AS_DATAFILE;
			}
			if (IsImageDataFile())
			{
				m_LoadFlags |= LOAD_LIBRARY_AS_IMAGE_RESOURCE;
			}
			return true;
		}
	}
	return false;
}
bool KxLibrary::Load(const wxString& libraryPath, DWORD flags)
{
	if (!IsOK())
	{
		m_FilePath = libraryPath;
		m_LoadFlags = flags;
		m_Handle = ::LoadLibraryExW(libraryPath.wc_str(), nullptr, flags);
		m_OwnHandle = true;

		return IsOK();
	}
	return false;
}
void KxLibrary::Unload()
{
	if (m_Handle && m_OwnHandle)
	{
		::FreeLibrary(m_Handle);
	}

	m_Handle = nullptr;
	m_OwnHandle = false;
	m_FilePath.Clear();
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
void KxLibrary::SaveVersionString(const KxLibraryVersionInfo& info, const wxString& queryTemplate, const KxFramework::UntypedMemorySpan& buffer, const wxString& rawFiledName, const wxString& infoFiledName)
{
	wxString query = wxString::Format(wxS("%s\\%s"), queryTemplate, rawFiledName);
	UINT size = 0;
	LPWSTR stringInfo = nullptr;
	if (::VerQueryValueW(buffer.data(), query.wc_str(), (void**)&stringInfo, &size) && stringInfo)
	{
		wxString value = info.GetString(rawFiledName);
		if (value.IsEmpty())
		{
			value = info.GetString(infoFiledName);
		}
		memset(stringInfo, 0, sizeof(WCHAR) * size);
		wcsncpy(stringInfo, value.wc_str(), size);
	}
}
void KxLibrary::LoadVersionString(KxLibraryVersionInfo& info, const wxString& queryTemplate, const KxFramework::UntypedMemorySpan& buffer, const wxString& rawFiledName, const wxString& infoFiledName)
{
	wxString query = wxString::Format(wxS("%s\\%s"), queryTemplate, rawFiledName);
	UINT size = 0;
	LPWSTR stringInfo = nullptr;
	if (::VerQueryValueW(buffer.data(), query.wc_str(), (void**)&stringInfo, &size) && stringInfo)
	{
		info.SetString(infoFiledName, wxString(stringInfo, size));
	}
}
wxMemoryBuffer KxLibrary::CreateVersionInfoStruct(const wxString& templateString, const KxLibraryVersionInfo& info)
{
	DWORD structSize = ::GetFileVersionInfoSizeW(templateString.wc_str(), nullptr);
	if (structSize != 0)
	{
		wxMemoryBuffer buffer(structSize);
		BOOL ret = ::GetFileVersionInfoW(templateString.wc_str(), 0, structSize, buffer.GetData());
		if (ret)
		{
			void* fixedInfo = nullptr;
			UINT size = 0;
			ret = ::VerQueryValueW(buffer.GetData(), L"\\", &fixedInfo, &size);
			if (ret != 0 && fixedInfo != nullptr)
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
			void* langInfo = nullptr;
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
	DWORD structSize = ::GetFileVersionInfoSizeW(filePath.wc_str(), nullptr);
	if (structSize != 0)
	{
		wxMemoryBuffer buffer(structSize);
		BOOL ret = ::GetFileVersionInfoW(filePath.wc_str(), 0, structSize, buffer.GetData());
		if (ret)
		{
			void* fixedInfo = nullptr;
			UINT size = 0;
			ret = ::VerQueryValueW(buffer.GetData(), L"\\", &fixedInfo, &size);
			if (ret != 0 && fixedInfo != nullptr)
			{
				info.MarkAsInit();

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
			void* langInfo = nullptr;
			size = 0;
			ret = ::VerQueryValueW(buffer.GetData(), L"\\VarFileInfo\\Translation", &langInfo, &size);
			if (ret)
			{
				info.MarkAsInit();
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
	return m_Handle || !m_FilePath.IsEmpty();
}

bool KxLibrary::IsDataFile() const
{
	return Util::IsLibraryDataFile(m_Handle);
}
bool KxLibrary::IsImageDataFile() const
{
	return Util::IsLibraryImageResourse(m_Handle);
}
bool KxLibrary::IsResource() const
{
	return Util::IsLibraryResource(m_Handle);
}

HMODULE KxLibrary::GetHandle() const
{
	return m_Handle;
}
void KxLibrary::SetHandle(HMODULE handle)
{
	m_Handle = handle;
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
KxIntVector KxLibrary::EnumResourceLanguages(const wxString& type, const wxString& name) const
{
	KxIntVector list;

	Util::CallbackInfo info(const_cast<KxLibrary*>(this), &list);
	EnumResourceLanguagesW(m_Handle, Util::GetNameOrID(type), Util::GetNameOrID(name), (ENUMRESLANGPROCW)Util::EnumResourceLanguagesProc, (LONG_PTR)&info);
	return list;
}
KxAnyVector KxLibrary::EnumResourceTypes(WORD localeID) const
{
	KxAnyVector list;

	Util::CallbackInfo info(const_cast<KxLibrary*>(this), &list);
	EnumResourceTypesExW(m_Handle, Util::EnumResourceTypesProc, (LONG_PTR)&info, 0, Util::GetLangID(localeID));
	return list;
}
KxAnyVector KxLibrary::EnumResources(const wxString& type, WORD localeID) const
{
	KxAnyVector list;

	Util::CallbackInfo info(const_cast<KxLibrary*>(this), &list);
	EnumResourceNamesExW(m_Handle, Util::GetNameOrID(type), Util::EnumResourcesProc, (LONG_PTR)&info, 0, Util::GetLangID(localeID));
	return list;
}
KxFramework::UntypedMemorySpan KxLibrary::GetResource(const wxString& type, const wxString& name, WORD localeID) const
{
	return Util::GetResource(m_Handle, Util::GetResourceHandle(m_Handle, type, name, localeID));
}

wxBitmap KxLibrary::GetBitmap(const wxString& name, WORD localeID) const
{
	return Util::LoadGDIImage<wxBitmap>(m_Handle, name, ResIDToName(RT_BITMAP), IMAGE_BITMAP, Util::DefaultIconSize, localeID);
}
wxIcon KxLibrary::GetIcon(const wxString& name, wxSize size, WORD localeID) const
{
	return Util::LoadGDIImage<wxIcon>(m_Handle, name, ResIDToName(RT_GROUP_ICON), IMAGE_ICON, size, localeID);
}
wxIcon KxLibrary::GetIcon(const wxString& name, size_t index, WORD localeID) const
{
	KxFramework::UntypedMemorySpan groupBuffer = Util::GetResource(m_Handle, Util::GetResourceHandle(m_Handle, ResIDToName(RT_GROUP_ICON), name, localeID));
	if (!groupBuffer.empty())
	{
		Util::IconGroupDirectory* iconGroup = (Util::IconGroupDirectory*)groupBuffer.data();
		WORD iconCount = iconGroup->idCount;
		if (index >= iconCount)
		{
			index = iconCount - 1;
		}

		Util::IconGroupEntry* iconInfo = &iconGroup->idEntries[index];
		WORD imageID = iconInfo->id;

		KxFramework::UntypedMemorySpan iconBuffer = Util::GetResource(m_Handle, Util::GetResourceHandle(m_Handle, ResIDToName(RT_ICON), ResIDToName(imageID), localeID));
		if (!iconBuffer.empty())
		{
			int width = 0;
			int height = 0;

			// I don't remember what '0x00030000' is.
			HICON iconHandle = ::CreateIconFromResourceEx((PBYTE)iconBuffer.data(), iconBuffer.size(), TRUE, 0x00030000, width, height, LR_DEFAULTCOLOR);
			if (iconHandle != nullptr)
			{
				wxIcon icon;
				icon.CreateFromHICON(iconHandle);
				return icon;
			}
		}
	}
	return wxNullIcon;
}
size_t KxLibrary::GetIconCount(const wxString& name, WORD localeID) const
{
	KxFramework::UntypedMemorySpan groupBuffer = Util::GetResource(m_Handle, Util::GetResourceHandle(m_Handle, ResIDToName(RT_GROUP_ICON), name));
	if (!groupBuffer.empty())
	{
		Util::IconGroupDirectory* iconGroup = (Util::IconGroupDirectory*)groupBuffer.data();
		return iconGroup->idCount;
	}
	return 0;
}
wxCursor KxLibrary::GetCursor(const wxString& name, WORD localeID) const
{
	return Util::LoadGDIImage<wxCursor>(m_Handle, name, ResIDToName(RT_CURSOR), IMAGE_CURSOR, Util::DefaultIconSize, localeID);
}
wxString KxLibrary::GetString(const wxString& name, WORD localeID) const
{
	if (localeID == 0)
	{
		// LoadString can not load strings specified by locale ID
		long stringID = 0;
		name.ToLong(&stringID);

		LPCWSTR string = nullptr;
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
			KxFramework::UntypedMemorySpan data = GetResource(ResIDToName(RT_STRING), ResIDToName(MAKEINTRESOURCEW(stringID / 16 + 1)), localeID);
			if (!data.empty())
			{
				stringID = stringID % 16;
				DWORD tableSize = data.size() / sizeof(WCHAR);
				DWORD offset = 0;
				DWORD index = 0;

				const WCHAR* stringData = (const WCHAR*)data.data();
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
	return {};
}

wxString KxLibrary::FormatMessage(DWORD messageID, WORD localeID) const
{
	// TODO: Add new formatting function.
	//return KxSystem::FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_HMODULE, m_Handle, messageID, Util::GetLangID(localeID));
	return {};
}

bool KxLibrary::IsResourceExist(const wxString& type, const wxString& name, WORD localeID)
{
	return Util::GetResourceHandle(m_Handle, type, name, localeID) != nullptr;
}
bool KxLibrary::RemoveResource(const wxString& type, const wxString& name, WORD localeID, bool updateNow)
{
	KxLibraryUpdateLocker locker(this);
	if (locker.IsOK())
	{
		locker.UpdateResource(type, name, {}, localeID);
	}
	return locker.IsSuccess();
}
bool KxLibrary::UpdateResource(const wxString& type, const wxString& name, const KxFramework::UntypedMemorySpan& data, bool overwrite, WORD localeID, bool updateNow)
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

	HANDLE fileHandle = ::CreateFileW(GetFileName().wc_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		HANDLE fileMapping = ::CreateFileMappingW(fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (fileMapping != nullptr)
		{
			LPVOID fileBase = ::MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
			if (fileBase != nullptr)
			{
				PIMAGE_DOS_HEADER headerDOS = (PIMAGE_DOS_HEADER)fileBase;
				PIMAGE_NT_HEADERS headerNT = (PIMAGE_NT_HEADERS)((size_t)headerDOS + (size_t)headerDOS->e_lfanew);
				if (!(::IsBadReadPtr(headerNT, sizeof(IMAGE_NT_HEADERS)) || headerNT->Signature != IMAGE_NT_SIGNATURE))
				{
					#pragma warning(suppress: 4312)
					PIMAGE_EXPORT_DIRECTORY exportDir = (PIMAGE_EXPORT_DIRECTORY)headerNT->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
					if (exportDir)
					{
						exportDir = (PIMAGE_EXPORT_DIRECTORY)::ImageRvaToVa(headerNT, headerDOS, (size_t)exportDir, 0);

						#pragma warning(suppress: 4312)
						DWORD** namesArray = (DWORD**)exportDir->AddressOfNames;
						namesArray = (DWORD**)::ImageRvaToVa(headerNT, headerDOS, (size_t)namesArray, 0);
						if (namesArray != nullptr)
						{
							DWORD functionsCount = exportDir->NumberOfNames;
							functionNames.reserve(functionsCount);
							for (DWORD i = 0; i < functionsCount; i++)
							{
								#pragma warning(suppress: 4302)
								#pragma warning(suppress: 4311)
								const char* name = (const char*)::ImageRvaToVa(headerNT, headerDOS, (DWORD)(*namesArray), 0);
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

void* KxLibrary::GetProcAddress(const char* name) const
{
	return ::GetProcAddress(m_Handle, name);
}
void* KxLibrary::GetProcAddress(const wchar_t* name) const
{
	wxString nameCopy(name);
	return ::GetProcAddress(m_Handle, nameCopy.c_str());
}
void* KxLibrary::GetProcAddress(const wxString& name) const
{
	return ::GetProcAddress(m_Handle, name.c_str());
}
void* KxLibrary::GetProcAddress(uint32_t ordinal) const
{
	return ::GetProcAddress(m_Handle, MAKEINTRESOURCEA(ordinal));
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
