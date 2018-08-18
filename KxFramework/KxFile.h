#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxLibrary.h"
#include <DbgHelp.h>
#include <ACLAPI.h>
#include <SDDL.h>
#include <SHLWAPI.h>
#include "KxFramework/KxWinUndef.h"
#include <wx/filename.h>

enum KxFileNamespace
{
	KxFNS_MIN = -1,

	KxFNS_NULL,
	KxFNS_WIN32_FILE,
	KxFNS_WIN32_DEVICE,
	KxFNS_NT,
	KxFNS_WIN32_FILE_UNC,
	KxFNS_WIN32_VOLUME,
	KxFNS_NETWORK,
	KxFNS_NETWORK_UNC,

	KxFNS_MAX
};
enum KxFileSearchType
{
	KxFS_FILE = 1,
	KxFS_FOLDER = 2,
	KxFS_ALL = KxFS_FILE|KxFS_FOLDER
};
enum KxFileBinaryFormat
{
	KxFBF_WIN16,
	KxFBF_WIN32,
	KxFBF_WIN64,
	KxFBF_WIN64_IA,
	KxFBF_WIN_NT,
	KxFBF_DOS,
	KxFBF_PIF,
	KxFBF_OS216,
	KxFBF_OS232,
	KxFBF_POSIX,
	KxFBF_PM16,
	KxFBF_PM32,

	KxFBF_INVALID = -1
};
enum KxFileTime
{
	KxFILETIME_CREATION,
	KxFILETIME_MODIFICATION,
	KxFILETIME_LAST_ACCESS,
};

class KxMenu;
class KxFile
{
	public:
		struct SplitPathData
		{
			wxString Drive;
			wxString Folder;
			wxString FileName;
			wxString Ext;
		};

	public:
		static wxString GetFullPathName(const wxString& filePath);
		static wxString GetLongPathName(const wxString& filePath);
		
		static wxString& TrimPath(wxString& path);
		static const wxString& GetNameSpaceString(KxFileNamespace namespaceType);
		static KxFileNamespace RemoveNamespacePrefix(wxString& path);
		static int64_t GetFileSize(const wxString& path);
		static wxString FormatFileSize(int64_t size, int precision = 0, const wxString& failMassage = wxEmptyString);
		static wxString MakePath(const SplitPathData& data);
		static wxString GetForbiddenChars()
		{
			return wxFileName::GetForbiddenChars();
		}
		static wxString CreateTempFile(const wxString& sRootPath = wxEmptyString);
		
		static bool IsFilePathExist(const wxString& path)
		{
			DWORD attributes = ::GetFileAttributesW(path);
			return (attributes != INVALID_FILE_ATTRIBUTES) && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
		}
		static bool IsFolderPathExist(const wxString& path)
		{
			DWORD attributes = ::GetFileAttributesW(path);
			return (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_DIRECTORY);
		}
		static bool IsPathExist(const wxString& path)
		{
			return ::GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES;
		}

		static wxString GetCWD();
		static bool SetCWD(const wxString& cwd);

		static wxString GetLongPath(const wxString& shortName);

	private:
		wxString m_Path;
		wxEvtHandler* m_EventHnadler = NULL;
		KxFileNamespace m_NameSpace = KxFNS_WIN32_FILE;

	private:
		bool CheckValidity() const;
		wxString NormalizePath(const wxString& path);
		void Init();

	public:
		static const wxString NullFilter;

		KxFile();
		KxFile(const SplitPathData& data);
		KxFile(const wxString& fileName, KxFileNamespace namespaceType = KxFNS_WIN32_FILE);
		virtual ~KxFile();

	public:
		bool IsOK() const
		{
			return !m_Path.IsEmpty();
		}

		bool HasEventHandler() const
		{
			return m_EventHnadler != NULL;
		}
		wxEvtHandler* GetEventHandler() const
		{
			return m_EventHnadler;
		}
		void SetEventHandler(wxEvtHandler* eventHandler)
		{
			m_EventHnadler = eventHandler;
		}

		// File names and parts
		wxString GetFullPath() const;
		wxString GetFullPathNS() const;

		wxString GetShortPath() const;
		wxString GetLongPath() const;

		wxString AbbreviatePath(size_t max) const;
		bool IsRelative() const
		{
			return GetDrive().IsEmpty();
		}

		wxString GetDrive() const;
		wxString GetFolders(int start = 0, int end = -1) const;
		KxStringVector GetFoldersArray() const;
		wxString GetFolder(int index) const;
		wxString GetName() const;
		wxString GetExt() const;
		wxString GetFullName() const;
		SplitPathData SplitPath() const;

		// Properties
		int64_t GetFileSize() const;
		int64_t GetFolderSize() const;
		wxString GetFormattedFileSize(int precision = 0, const wxString& failMassage = wxEmptyString) const;
		wxString GetFormattedFolderSize(int precision = 0, const wxString& failMassage = wxEmptyString) const;
		
		uint32_t GetAttributes() const;
		bool HasAttribute(uint32_t attribute) const;
		bool SetAttribute(uint32_t attribute, bool set);
		bool SetAttributes(uint32_t attributes);

		bool IsInUse() const;
		bool IsFile() const;
		bool IsFolder() const;
		
		KxFileBinaryFormat GetBinaryType() const;
		KxLibraryVersionInfo GetVersionInfo() const;

		// File time
		wxDateTime GetFileTime(KxFileTime type) const;
		bool SetFileTime(const wxDateTime& t, KxFileTime type);
		bool SetFileTime(const wxDateTime& tCreation, const wxDateTime& tModification, const wxDateTime& tLastAccess);

		// Existence
		bool IsFileExist() const;
		bool IsFolderExist() const;
		bool IsExist() const;

		// Search
		KxStringVector Find(const wxString& filter = NullFilter, KxFileSearchType elementType = KxFS_FILE, bool recurse = false) const;
		KxStringVector Find(const KxStringVector& filters, KxFileSearchType elementType = KxFS_FILE, bool recurse = false) const;

		// Copying, moving, removing etc
		bool CopyFile(const KxFile& destination, bool overwrite);
		bool MoveFile(const KxFile& destination, bool overwrite);
		bool RemoveFile(bool toRecycleBin = false);
		void CopyFolder(const wxString& filter, const KxFile& destination, bool recurse, bool overwrite);
		bool RemoveFolder(bool removeRootFolder = true, bool toRecycleBin = false);
		bool RemoveFolderTree(bool removeRootFolder = true, bool toRecycleBin = false);
		bool CreateFolder() const;
		bool Rename(const KxFile& destination, bool overwrite);

		// Misc
		bool ShellOpen();
		KxMenu* GetShellMenu();
		
	public:
		bool operator==(const KxFile& other);
		bool operator!=(const KxFile& other);
};

//////////////////////////////////////////////////////////////////////////
class KxEvtFile: public wxEvtHandler, public KxFile
{
	private:
		void SetEventHandler(wxEvtHandler* eventHandler) = delete;

	private:
		void Init()
		{
			KxFile::SetEventHandler(this);
		}

	public:
		KxEvtFile()
			:KxFile()
		{
			Init();
		}
		KxEvtFile(const SplitPathData& data)
			:KxFile(data)
		{
			Init();
		}
		KxEvtFile(const wxString& fileName, KxFileNamespace namespaceType = KxFNS_WIN32_FILE)
			:KxFile(fileName, namespaceType)
		{
			Init();
		}
};
