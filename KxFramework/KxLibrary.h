#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/System/UndefWindows.h"
#include "Kx/General/MemorySpan.h"
class KxLibraryUpdateLocker;

class KX_API KxLibraryVersionInfo
{
	public:
		using StringsListType = std::unordered_map<wxString, wxString>;
		
		struct VerQueryValueLangInfo
		{
			WORD Language;
			WORD CodePage;
		};
		struct LangInfoStruct
		{
			DWORD ID;
			DWORD CodePage;
			wxString Name;
		};
		struct FlagsStruct
		{
			DWORD FlagsMask;
			DWORD Flags;
			DWORD OS;
			DWORD FileType;
			DWORD DriverInfo;
			DWORD FontInfo;
			DWORD VXDInfo;
		};

		static const LPCWSTR VersionInfoRawFieldNames[];
		static const LPCWSTR VersionInfoFieldNames[];
		static const size_t VersionInfoFieldNamesCount;

	public:
		LangInfoStruct LangInfo = {0};
		FlagsStruct FlagsInfo = {0};

	private:
		StringsListType m_Strings;
		bool m_IsInit = false;
		
	public:
		KxLibraryVersionInfo(size_t initialCount = 10)
		{
			m_Strings.reserve(initialCount);
			FlagsInfo.DriverInfo = (DWORD)-1;
			FlagsInfo.FontInfo = (DWORD)-1;
			FlagsInfo.VXDInfo = (DWORD)-1;
		}

	public:
		bool IsOK() const
		{
			return m_IsInit;
		}
		void MarkAsInit()
		{
			m_IsInit = true;
		}
		
		const StringsListType& GetStringsList() const
		{
			return m_Strings;
		}
		size_t GetStringsCount() const
		{
			return m_Strings.size();
		}
		
		bool HasString(const wxString& name) const
		{
			return m_Strings.count(name);
		}
		wxString GetString(const wxString& name) const
		{
			if (HasString(name))
			{
				return m_Strings.at(name);
			}
			return {};
		}
		void SetString(const wxString& name, const wxString& value = {})
		{
			m_Strings[name] = value;
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxLibrary
{
	friend class KxLibraryUpdateLocker;

	public:
		static const WORD DefaultLocaleID;
		static const wxSize DefaultIconSize;

	public:
		static bool SetSearchFolder(const wxString& path);
		static const void* AddSearchFolder(const wxString& path);
		static bool RemoveSearchFolder(const void* pathCookie);
		
		template<class T> static wxString ResIDToName(T id)
		{
			// Don't touch C-style cast
			return wxString::Format("%zu", (size_t)id);
		}

	private:
		wxString m_FilePath;
		HMODULE m_Handle = nullptr;
		DWORD m_LoadFlags = 0;
		bool m_OwnHandle = false;

	private:
		void SetHandle(HMODULE handle);

	public:
		KxLibrary() = default;
		KxLibrary(HMODULE libraryHandle)
		{
			Load(libraryHandle);
		}
		KxLibrary(const wxString& libraryPath, DWORD flags = 0)
		{
			Load(libraryPath, flags);
		}
		virtual ~KxLibrary()
		{
			Unload();
		}

	public:
		bool Load(HMODULE libraryHandle);
		bool Load(const wxString& libraryPath, DWORD flags = 0);
		void Unload();

	public:
		// Version info
		static wxString NumberVersionToString(DWORD mostSignificant, DWORD leastSignificant);
		static void StringVersionToNumber(const wxString& version, DWORD& mostSignificant, DWORD& leastSignificant);
		static void SaveVersionString(const KxLibraryVersionInfo& info, const wxString& queryTemplate, const KxFramework::UntypedMemorySpan& buffer, const wxString& rawFiledName, const wxString& infoFiledName);
		static void LoadVersionString(KxLibraryVersionInfo& info, const wxString& queryTemplate, const KxFramework::UntypedMemorySpan& buffer, const wxString& rawFiledName, const wxString& infoFiledName);
		static wxMemoryBuffer CreateVersionInfoStruct(const wxString& templateString, const KxLibraryVersionInfo& info);
		static KxLibraryVersionInfo GetVersionInfoFromFile(const wxString& filePath);

		// Properties
		bool IsOK() const;

		bool IsDataFile() const;
		bool IsImageDataFile() const;
		bool IsResource() const;

		HMODULE GetHandle() const;
		wxString GetFileName() const;
		DWORD GetLoadFlags() const;

		// Resources
		KxIntVector EnumResourceLanguages(const wxString& type, const wxString& name) const;
		KxAnyVector EnumResourceTypes(WORD localeID = DefaultLocaleID) const;
		KxAnyVector EnumResources(const wxString& type, WORD localeID = DefaultLocaleID) const;
		KxFramework::UntypedMemorySpan GetResource(const wxString& type, const wxString& name, WORD localeID = DefaultLocaleID) const;
		
		wxBitmap GetBitmap(const wxString& name, WORD localeID = DefaultLocaleID) const;
		wxIcon GetIcon(const wxString& name, wxSize size = DefaultIconSize, WORD localeID = DefaultLocaleID) const;
		wxIcon GetIcon(const wxString& name, size_t index = 0, WORD localeID = DefaultLocaleID) const;
		size_t GetIconCount(const wxString& name, WORD localeID = DefaultLocaleID) const;
		wxCursor GetCursor(const wxString& name, WORD localeID = DefaultLocaleID) const;
		wxString GetString(const wxString& name, WORD localeID = DefaultLocaleID) const;
		
		bool RemoveResource(const wxString& type, const wxString& name, WORD localeID = DefaultLocaleID, bool updateNow = false);
		bool IsResourceExist(const wxString& type, const wxString& name, WORD localeID = DefaultLocaleID);
		bool UpdateResource(const wxString& type, const wxString& name, const KxFramework::UntypedMemorySpan& data, bool overwrite = false, WORD localeID = DefaultLocaleID, bool updateNow = false);
		
		wxString FormatMessage(DWORD messageID, WORD localeID = DefaultLocaleID) const;

		// Functions
		KxStringVector EnumFunctions() const;
		
		void* GetProcAddress(const char* name) const;
		void* GetProcAddress(const wchar_t* name) const;
		void* GetProcAddress(const wxString& name) const;
		void* GetProcAddress(uint32_t ordinal) const;
};
