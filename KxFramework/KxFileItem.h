#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFile.h"
#include <KxFramework/KxUtility.h>
class KX_API KxFileFinder;

class KX_API KxFileItem final
{
	friend class KxFileFinder;

	private:
		wxString m_Name;
		wxString m_Source;
		wxDateTime m_CreationTime;
		wxDateTime m_LastAccessTime;
		wxDateTime m_ModificationTime;
		int64_t m_FileSize = -1;
		int64_t m_CompressedFileSize = -1;
		uint32_t m_Attributes = INVALID_FILE_ATTRIBUTES;
		uint32_t m_ReparsePointAttributes = 0;
		intptr_t m_ExtraData = -1;

	private:
		void MakeNull(bool attribuesOnly = false);
		void Set(const WIN32_FIND_DATAW& fileInfo);
		void SetTime(const FILETIME& fileTime, wxDateTime& fileTimeWx) const;
		bool DoUpdateInfo(const wxString& fullPath);

	public:
		KxFileItem() = default;
		KxFileItem(const wxString& fullPath);
		KxFileItem(const wxString& source, const wxString& fileName);
		
		KxFileItem(const KxFileItem&) = default;
		KxFileItem(KxFileItem&& other)
		{
			*this = std::move(other);
		}

	private:
		KxFileItem(const KxFileFinder& finder, const WIN32_FIND_DATAW& fileInfo);

	public:
		bool IsOK() const
		{
			return m_Attributes != INVALID_FILE_ATTRIBUTES;
		}
		bool UpdateInfo()
		{
			return DoUpdateInfo(GetFullPath());
		}

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
		void SetAttributes(uint32_t attributes)
		{
			m_Attributes = attributes;
		}
		void SetNormalAttributes()
		{
			m_Attributes = FILE_ATTRIBUTE_NORMAL;
		}
		uint32_t GetReparsePointAttributes() const
		{
			return m_ReparsePointAttributes;
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

		int64_t GetCompressedFileSize() const
		{
			return m_CompressedFileSize;
		}
		void SetCompressedFileSize(int64_t size)
		{
			m_CompressedFileSize = size;
		}

		bool IsCompressed() const
		{
			return m_Attributes & FILE_ATTRIBUTE_COMPRESSED;
		}
		double GetCompressionRatio() const
		{
			if (m_FileSize > 0)
			{
				return (double)m_CompressedFileSize / m_FileSize;
			}
			return -1;
		}

		wxString GetSource() const
		{
			return m_Source;
		}
		void SetSource(const wxString& source)
		{
			m_Source = source;
		}
		
		wxString GetName() const
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
			return m_Source + wxS('\\') + m_Name;
		}
		void SetFullPath(const wxString& fullPath)
		{
			m_Source = fullPath.BeforeLast(wxS('\\'), &m_Name);
		}

	public:
		KxFileItem& operator=(const KxFileItem&) = default;
		KxFileItem& operator=(KxFileItem&& other)
		{
			*this = other;
			other.MakeNull();

			return *this;
		}
};
