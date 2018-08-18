#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxIOwned.h"
#include "KxFramework/KxWinUndef.h"

class KxLibraryVersionInfo
{
	public:
		typedef std::unordered_map<wxString, wxString> StringsListType;
		typedef struct
		{
			WORD Language;
			WORD CodePage;
		} VerQueryValueLangInfo;
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
		StringsListType StringsList;
		bool Init = false;
		
	public:
		KxLibraryVersionInfo(size_t initialCount = 10)
		{
			StringsList.reserve(initialCount);
			FlagsInfo.DriverInfo = (DWORD)-1;
			FlagsInfo.FontInfo = (DWORD)-1;
			FlagsInfo.VXDInfo = (DWORD)-1;
		}

		bool IsOK() const
		{
			return Init;
		}
		void SetInit()
		{
			Init = true;
		}
		const StringsListType& GetStringsList() const
		{
			return StringsList;
		}
		size_t GetStringsCount() const
		{
			return StringsList.size();
		}
		bool HasString(const wxString& name) const
		{
			return StringsList.count(name);
		}
		const wxString& GetString(const wxString& name) const
		{
			if (HasString(name))
			{
				return StringsList.at(name);
			}
			return wxNullString;
		}
		void SetString(const wxString& name, const wxString& value = wxEmptyString)
		{
			StringsList[name] = value;
		}
};

class KxLibraryUpdateLocker;
class KxLibrary: public KxIOwnedSimple
{
	friend class KxLibraryUpdateLocker;

	private:
		struct CallbackInfo
		{
			KxLibrary* self = NULL;
			void* Data = NULL;

			CallbackInfo(KxLibrary* self, void* data = NULL)
				:self(self), Data(data)
			{
			}
		};
		static BOOL CALLBACK EnumResourcesProc(HMODULE moduleHandle, LPCTSTR resType, LPTSTR resName, LONG_PTR lParam);
		static BOOL CALLBACK EnumResourceTypesProc(HMODULE moduleHandle, LPTSTR resType, LONG_PTR lParam);
		static BOOL CALLBACK EnumResourceLanguagesProc(HMODULE moduleHandle, LPCTSTR resType, LPTSTR resName, WORD langID, LONG_PTR lParam);

		static WORD GetLangID(BYTE primaryID, BYTE secondaryID);
		static WORD GetLangID(WORD localeID);
		static LPCWSTR GetNameOrID(const wxString& name);
		HRSRC GetResourceHandle(const wxString& type, const wxString& name, WORD localeID = DefaultLocaleID) const;
		HGLOBAL LoadResource(HRSRC hResHandle) const;
		wxMemoryBuffer GetResource(HRSRC hResHandle) const;
		void SetHandle(HMODULE hHandle);
		bool UpdateResource(const wxString& type, const wxString& name, const wxMemoryBuffer& data, WORD localeID, bool updateNow = false);

		HANDLE LoadGDIImageAux(const wxString& name, const wxString& type, UINT nGDIType, wxSize size = DefaultIconSize, WORD localeID = DefaultLocaleID) const
		{
			HGLOBAL resDataHandle = LoadResource(GetResourceHandle(type, name, localeID));
			if (resDataHandle)
			{
				return LoadImageW(m_Handle, GetNameOrID(name), nGDIType, size.GetWidth(), size.GetHeight(), LR_DEFAULTCOLOR);
			}
			return NULL;
		}
		template<class T> T LoadGDIImage(const wxString& name, const wxString& type, UINT GDIType, wxSize size = DefaultIconSize, WORD localeID = DefaultLocaleID) const
		{
			HANDLE imageHandle = LoadGDIImageAux(name, type, GDIType, size, localeID);
			if (imageHandle)
			{
				T image;
				image.SetHandle(imageHandle);
				if (size.IsFullySpecified())
				{
					image.SetWidth(size.GetWidth());
					image.SetHeight(size.GetHeight());
				}
				return image;
			}
			return T();
		}
		template<> wxIcon LoadGDIImage<wxIcon>(const wxString& name, const wxString& type, UINT nGDIType, wxSize size, WORD localeID) const
		{
			HANDLE imageHandle = LoadGDIImageAux(name, type, nGDIType, size, localeID);
			if (imageHandle)
			{
				wxIcon icon;
				icon.CreateFromHICON((HICON)imageHandle);
				return icon;
			}
			return wxNullIcon;
		}
		
		#pragma pack( push )
		#pragma pack( 2 )
		typedef struct
		{
			BYTE   bWidth;               // Width, in pixels, of the image
			BYTE   bHeight;              // Height, in pixels, of the image
			BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
			BYTE   bReserved;            // Reserved
			WORD   wPlanes;              // Color Planes
			WORD   wBitCount;            // Bits per pixel
			DWORD  dwBytesInRes;         // how many bytes in this resource?
			WORD   id;                  // the ID
		} IconGroupEntry;
		#pragma pack( pop )

		#pragma pack( push )
		#pragma pack( 2 )
		typedef struct
		{
			WORD            idReserved;   // Reserved (must be 0)
			WORD            idType;       // Resource type (1 for icons)
			WORD            idCount;      // How many images?
			IconGroupEntry idEntries[1]; // The entries for each image
		} IconGroupDirectory;
		#pragma pack( pop )

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
		HMODULE m_Handle = NULL;
		wxString m_FilePath;
		DWORD m_LoadFlags = DefaultLoadFlags;
		HANDLE m_UpdateHandle = NULL;

	public:
		static const DWORD DefaultLoadFlags = 0;
		static const WORD DefaultLocaleID;
		static const wxSize DefaultIconSize;

		KxLibrary();
		KxLibrary(HMODULE hLibraryHandle);
		KxLibrary(const wxString& libraryPath, DWORD flags = DefaultLoadFlags);
		virtual ~KxLibrary();

	public:
		bool Load(HMODULE hLibraryHandle);
		bool Load(const wxString& libraryPath, DWORD flags = DefaultLoadFlags);
		void Unload();

	public:
		// Version info
		static wxString NumberVersionToString(DWORD mostSignificant, DWORD leastSignificant);
		static void StringVersionToNumber(const wxString& version, DWORD& mostSignificant, DWORD& leastSignificant);
		static void SaveVersionString(const KxLibraryVersionInfo& info, const wxString& queryTemplate, const wxMemoryBuffer& buffer, const wxString& rawFiledName, const wxString& infoFiledName);
		static void LoadVersionString(KxLibraryVersionInfo& info, const wxString& queryTemplate, const wxMemoryBuffer& buffer, const wxString& rawFiledName, const wxString& infoFiledName);
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
		bool BeginUpdateResource();
		bool EndUpdateResource(bool noReopen = false);
		bool IsUpdatingResources() const;

		KxIntVector EnumResourceLanguages(const wxString& type, const wxString& name) const;
		KxAnyVector EnumResourceTypes(WORD localeID = DefaultLocaleID) const;
		KxAnyVector EnumResources(const wxString& type, WORD localeID = DefaultLocaleID) const;
		wxMemoryBuffer GetResource(const wxString& type, const wxString& name, WORD localeID = DefaultLocaleID) const;
		wxBitmap GetBitmap(const wxString& name, WORD localeID = DefaultLocaleID) const;
		wxIcon GetIcon(const wxString& name, wxSize size = DefaultIconSize, WORD localeID = DefaultLocaleID) const;
		wxIcon GetIcon(const wxString& name, size_t index = 0, WORD localeID = DefaultLocaleID) const;
		size_t GetIconCount(const wxString& name, WORD localeID = DefaultLocaleID) const;
		wxCursor GetCursor(const wxString& name, WORD localeID = DefaultLocaleID) const;
		wxString GetString(const wxString& name, WORD localeID = DefaultLocaleID) const;
		bool RemoveResource(const wxString& type, const wxString& name, WORD localeID = DefaultLocaleID, bool updateNow = false);
		bool IsResourceExist(const wxString& type, const wxString& name, WORD localeID = DefaultLocaleID);
		bool UpdateResource(const wxString& type, const wxString& name, const wxMemoryBuffer& data, bool overwrite = false, WORD localeID = DefaultLocaleID, bool updateNow = false);
		wxString FormatMessage(DWORD messageID, WORD localeID = DefaultLocaleID) const;

		// Functions
		KxStringVector EnumFunctions() const;
		void* GetProcAddress(const wxString& name) const;
		void* GetProcAddress(WORD nOrdinal) const;
		bool IsFunctionExist(const wxString& name) const;
		bool IsFunctionExist(WORD nOrdinal) const;
		#if defined RtCFunction
		RtCFunction* Function(const wxString& name, lua_State* L, const Lua::IntegerArray& tArgTypes, LClassID nRetTypeID, RtCFunctionABI nABI);
		RtCFunction* Function(WORD nOrdinal, lua_State* L, const Lua::IntegerArray& tArgTypes, LClassID nRetTypeID, RtCFunctionABI nABI);
		#endif
};

//////////////////////////////////////////////////////////////////////////
class KxLibraryUpdateLocker
{
	private:
		HANDLE m_UpdateHandle = NULL;
		bool m_Success = false;
		KxLibrary* m_Library = NULL;
		wxString m_LibarryFullPath;

	private:
		void Unlock();
		void Lock();

	public:
		KxLibraryUpdateLocker(KxLibrary* library);
		~KxLibraryUpdateLocker();

		void UpdateResource(const wxString& type, const wxString& name, const wxMemoryBuffer& data, WORD localeID);
		bool IsOK() const
		{
			return m_UpdateHandle != NULL && m_Library != NULL;
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
