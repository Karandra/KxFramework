#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFile.h"
#include <KxFramework/KxUtility.h>
class KxFileFinder;

class KxFileItem
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
		intptr_t m_ExtraData = -1;

	private:
		void MakeNull(bool attribuesOnly = false);
		void Set(const WIN32_FIND_DATAW& fileInfo);
		void SetTime(const FILETIME& fileTime, wxDateTime& fileTimeWx) const;

	public:
		KxFileItem() = default;
		KxFileItem(const wxString& fullPath);
		KxFileItem(const wxString& source, const wxString& fileName);

	private:
		KxFileItem(KxFileFinder* finder, const WIN32_FIND_DATAW& fileInfo);

	public:
		bool IsOK() const
		{
			return m_Attributes != INVALID_FILE_ATTRIBUTES;
		}
		bool UpdateInfo();

		template<class T> T GetExtraData() const
		{
			static_assert(sizeof(T) <= sizeof(m_ExtraData) && std::is_trivially_copyable_v<T>, "invalid data type");

			return static_cast<T>(m_ExtraData);
		}
		template<class T> void SetExtraData(const T& value)
		{
			static_assert(sizeof(T) <= sizeof(m_ExtraData) && std::is_trivially_copyable_v<T>, "invalid data type");

			m_ExtraData = static_cast<intptr_t>(value);
		}

		bool IsNormalItem() const
		{
			return IsOK() && !IsReparsePoint() && !IsCurrentOrParent();
		}
		bool IsCurrentOrParent() const;
		bool IsReparsePoint() const
		{
			return m_Attributes & FILE_ATTRIBUTE_REPARSE_POINT;
		}

		bool IsDirectory() const
		{
			return m_Attributes & FILE_ATTRIBUTE_DIRECTORY;
		}
		bool IsDirectoryEmpty() const;
		KxFileItem& SetDirectory()
		{
			KxUtility::ModFlagRef(m_Attributes, FILE_ATTRIBUTE_DIRECTORY, true);
			return *this;
		}

		bool IsFile() const
		{
			return !IsDirectory();
		}
		KxFileItem& SetFile()
		{
			KxUtility::ModFlagRef(m_Attributes, FILE_ATTRIBUTE_DIRECTORY, false);
			return *this;
		}
		bool IsElementType(KxFileSearchType type) const
		{
			return (type & KxFS_FILE && IsFile()) || (type & KxFS_FOLDER && IsDirectory());
		}
		
		bool IsReadOnly() const
		{
			return m_Attributes & FILE_ATTRIBUTE_READONLY;
		}
		KxFileItem& SetReadOnly(bool value = true)
		{
			KxUtility::ModFlagRef(m_Attributes, FILE_ATTRIBUTE_READONLY, value);
			return *this;
		}

		uint32_t GetAttributes() const
		{
			return m_Attributes;
		}
		uint32_t GetReparsePointAttributes() const
		{
			return m_ReparsePointAttributes;
		}
		void SetNormalAttributes()
		{
			m_Attributes = FILE_ATTRIBUTE_NORMAL;
		}

		wxDateTime GetCreationTime() const
		{
			return m_CreationTime;
		}
		void SetCreationTime(const wxDateTime& value)
		{
			m_CreationTime = value;
		}
		
		wxDateTime GetLastAccessTime() const
		{
			return m_LastAccessTime;
		}
		void SetLastAccessTime(const wxDateTime& value)
		{
			m_LastAccessTime = value;
		}
		
		wxDateTime GetModificationTime() const
		{
			return m_ModificationTime;
		}
		void SetModificationTime(const wxDateTime& value)
		{
			m_ModificationTime = value;
		}

		int64_t GetFileSize() const
		{
			return m_FileSize;
		}
		void SetFileSize(int64_t size)
		{
			m_FileSize = size;
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
		
		wxString GetFileExtension() const;
		void SetFileExtension(const wxString& ext);

		wxString GetFullPath() const
		{
			return m_Source + wxS('\\') + GetName();
		}
};
