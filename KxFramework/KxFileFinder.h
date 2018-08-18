#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFile.h"
class KxFileFinderItem;

class KxFileFinder
{
	public:
		static bool IsDirectoryEmpty(const wxString& directoryPath);

	private:
		wxEvtHandler* m_EvtHandler = NULL;
		const wxString m_Source;
		const wxString m_Filter;
		bool m_Canceled = false;

		HANDLE m_Handle = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATAW m_FindData = {0};

	private:
		bool OnFound(const WIN32_FIND_DATAW& fileInfo);
		wxString Normalize(const wxString& source, bool start, bool end) const;
		wxString ConstructSearchQuery() const;

	protected:
		virtual bool OnFound(const KxFileFinderItem& foundItem);

	public:
		KxFileFinder(const wxString& source, const wxString& filter = wxEmptyString, wxEvtHandler* eventHandler = NULL);
		virtual ~KxFileFinder();

	public:
		bool IsOK() const;
		bool IsCanceled() const
		{
			return m_Canceled;
		}
		bool Run();
		KxFileFinderItem FindNext();
		void NotifyFound(const KxFileFinderItem& foundItem)
		{
			m_Canceled = !OnFound(foundItem);
		}

		const wxString& GetSource() const
		{
			return m_Source;
		}
};

class KxFileFinderItem
{
	friend class KxFileFinder;

	private:
		wxString m_Source;
		wxString m_Name;
		uint32_t m_Attributes = INVALID_FILE_ATTRIBUTES;
		uint32_t m_ReparsePointAttributes = 0;
		wxDateTime m_CreationTime;
		wxDateTime m_LastAccessTime;
		wxDateTime m_ModificationTime;
		int64_t m_FileSize = -1;

	private:
		void MakeNull(bool bAttribuesOnly = false);
		void Set(const WIN32_FIND_DATAW& fileInfo);
		void SetTime(const FILETIME& fileTime, wxDateTime& wxTimeValue) const;

	public:
		KxFileFinderItem() {}
		KxFileFinderItem(const wxString& fullPath);
		KxFileFinderItem(const KxFileFinderItem& other) = default;
		~KxFileFinderItem();

	private:
		KxFileFinderItem(KxFileFinder* finder, const WIN32_FIND_DATAW& fileInfo);

	public:
		bool IsOK() const
		{
			return !m_Source.IsEmpty() && m_Attributes != INVALID_FILE_ATTRIBUTES;
		}

		bool IsNormalItem() const
		{
			return IsOK() && !IsReparsePoint() && !IsCurrentOrParent();
		}
		bool IsCurrentOrParent() const;
		bool IsDirectory() const
		{
			return m_Attributes & FILE_ATTRIBUTE_DIRECTORY;
		}
		bool IsDirectoryEmpty() const
		{
			return IsDirectory() && KxFileFinder::IsDirectoryEmpty(m_Source);
		}
		bool IsFile() const
		{
			return !IsDirectory();
		}
		bool IsElementType(KxFileSearchType type) const
		{
			return (type & KxFS_FILE && IsFile()) || (type & KxFS_FOLDER && IsDirectory());
		}
		bool IsReparsePoint() const
		{
			return m_Attributes & FILE_ATTRIBUTE_REPARSE_POINT;
		}

		uint32_t GetAttributes() const
		{
			return m_Attributes;
		}
		uint32_t GetReparsePointAttributes() const
		{
			return m_ReparsePointAttributes;
		}
		wxDateTime GetCreationTime() const
		{
			return m_CreationTime;
		}
		wxDateTime GetLastAccessTime() const
		{
			return m_LastAccessTime;
		}
		wxDateTime GetModificationTime() const
		{
			return m_ModificationTime;
		}
		int64_t GetFileSize() const
		{
			return m_FileSize;
		}
		
		const wxString& GetSource() const
		{
			return m_Source;
		}
		void SetSource(const wxString& source)
		{
			m_Source = source;
		}
		const wxString& GetName() const
		{
			return m_Name;
		}
		void SetName(const wxString& name)
		{
			m_Name = name;
		}
		wxString GetFullPath() const
		{
			return m_Source + '\\' + GetName();
		}

		bool UpdateInfo();
};