#pragma once
#include "Common.h"
#include "FSPath.h"
#include "Kx/General/BinarySize.h"
#include "Kx/General/UndefWindows.h"

namespace KxFramework
{
	class KX_API FileItem final
	{
		private:
			FSPath m_Source;
			wxDateTime m_CreationTime;
			wxDateTime m_LastAccessTime;
			wxDateTime m_ModificationTime;
			BinarySize m_FileSize;
			BinarySize m_CompressedFileSize;
			uint32_t m_Attributes = 0;
			uint32_t m_ReparsePointAttributes = 0;
			intptr_t m_ExtraData = -1;

		private:
			bool DoUpdateInfo();

		public:
			FileItem() = default;
			FileItem(const FSPath& fullPath)
				:m_Source(fullPath)
			{
				DoUpdateInfo();
			}
			FileItem(const FSPath& source, const FSPath& fileName)
				:m_Source(source)
			{
				m_Source.Append(fileName);
				DoUpdateInfo();
			}
			
			FileItem(const FileItem&) = default;
			FileItem(FileItem&&) = default;

		public:
			bool IsOK() const
			{
				return m_Attributes != INVALID_FILE_ATTRIBUTES;
			}
			bool UpdateInfo()
			{
				return DoUpdateInfo();
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

			BinarySize GetFileSize() const
			{
				return m_FileSize;
			}
			void SetFileSize(BinarySize size)
			{
				m_FileSize = size;
			}

			BinarySize GetCompressedFileSize() const
			{
				return m_CompressedFileSize;
			}
			void SetCompressedFileSize(BinarySize size)
			{
				m_CompressedFileSize = size;
			}

			bool IsCompressed() const
			{
				return m_Attributes & FILE_ATTRIBUTE_COMPRESSED;
			}
			double GetCompressionRatio() const
			{
				if (m_FileSize && m_CompressedFileSize)
				{
					return m_CompressedFileSize.GetBytes<double>() / m_FileSize.GetBytes<double>();
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
				return m_Source.GetName();
			}
			void SetName(const wxString& name)
			{
				m_Source.SetName(name);
			}
			
			wxString GetFileExtension() const
			{
				return m_Source.GetExtension();
			}
			void SetFileExtension(const wxString& ext)
			{
				m_Source.SetExtension(ext);
			}

			FSPath GetFullPath() const
			{
				return m_Source;
			}
			void SetFullPath(const FSPath& fullPath)
			{
				m_Source = fullPath;
			}

		public:
			FileItem& operator=(const FileItem&) = default;
			FileItem& operator=(FileItem&&) = default;

			explicit operator bool() const
			{
				return IsOK();
			}
			bool operator!() const
			{
				return !IsOK();
			}
	};
}
