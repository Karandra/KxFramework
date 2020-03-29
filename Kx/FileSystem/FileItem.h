#pragma once
#include "Common.h"
#include "FSPath.h"
#include "Kx/General/ExtraData.h"
#include "Kx/General/BinarySize.h"
#include "Kx/General/UndefWindows.h"

namespace KxFramework
{
	class IFileSystem;
}

namespace KxFramework
{
	class KX_API FileItem final: public TrivialExtraDataContainer
	{
		private:
			FSPath m_Path;
			BinarySize m_Size;
			BinarySize m_CompressedSize;
			wxDateTime m_CreationTime;
			wxDateTime m_LastAccessTime;
			wxDateTime m_ModificationTime;
			FileAttribute m_Attributes = FileAttribute::None;
			ReparsePointTag m_ReparsePointTags = ReparsePointTag::None;

		public:
			FileItem() = default;
			FileItem(FileItem&&) = default;
			FileItem(const FileItem&) = default;

			FileItem(const FSPath& fullPath)
				:m_Path(fullPath)
			{
			}
			FileItem(const FSPath& source, const FSPath& fileName)
				:m_Path(source)
			{
				m_Path.Append(fileName);
			}
			
			FileItem(const IFileSystem& fileSystem, const FSPath& fullPath)
				:FileItem(fullPath)
			{
				Refresh(fileSystem);
			}
			FileItem(const IFileSystem& fileSystem, const FSPath& source, const FSPath& fileName)
				:FileItem(source, fileName)
			{
				Refresh(fileSystem);
			}

		public:
			// General
			FileItem& Refresh(const IFileSystem& fileSystem);
			
			bool IsValid() const
			{
				return m_Path && m_Attributes != FileAttribute::Invalid;
			}
			bool IsNormalItem() const
			{
				return IsValid() && !IsReparsePoint() && !IsCurrentOrParent();
			}
			bool IsCurrentOrParent() const
			{
				if (m_Path.GetPathLength() >= 1)
				{
					const wxString name = m_Path.GetName();
					return name == wxS("..") || name == wxS('.');
				}
				return false;
			}

			// Attributes
			FileAttribute GetAttributes() const
			{
				return m_Attributes;
			}
			FileItem& SetAttributes(FileAttribute attributes)
			{
				m_Attributes = attributes;
				return *this;
			}
			
			ReparsePointTag GetReparsePointTags() const
			{
				return m_ReparsePointTags;
			}
			FileItem& SetReparsePointTags(ReparsePointTag tags)
			{
				m_ReparsePointTags = tags;
				return *this;
			}

			bool IsDirectory() const
			{
				return m_Attributes & FileAttribute::Directory;
			}
			bool IsCompressed() const
			{
				return m_Attributes & FileAttribute::Compressed;
			}
			bool IsReparsePoint() const
			{
				return m_Attributes & FileAttribute::ReparsePoint;
			}
			bool IsSymLink() const
			{
				return IsReparsePoint() && m_ReparsePointTags & ReparsePointTag::SymLink;
			}

			// Date and time
			wxDateTime GetCreationTime() const
			{
				return m_CreationTime;
			}
			FileItem& SetCreationTime(const wxDateTime& value)
			{
				m_CreationTime = value;
				return *this;
			}
			
			wxDateTime GetLastAccessTime() const
			{
				return m_LastAccessTime;
			}
			FileItem& SetLastAccessTime(const wxDateTime& value)
			{
				m_LastAccessTime = value;
				return *this;
			}
			
			wxDateTime GetModificationTime() const
			{
				return m_ModificationTime;
			}
			FileItem& SetModificationTime(const wxDateTime& value)
			{
				m_ModificationTime = value;
				return *this;
			}

			// Path and name
			FSPath GetFullPath() const
			{
				return m_Path;
			}
			FileItem& SetFullPath(const FSPath& fullPath)
			{
				m_Path = fullPath;
				return *this;
			}

			FSPath GetSource() const
			{
				return m_Path.GetParent();
			}
			FileItem& SetSource(const FSPath& source)
			{
				wxString name = m_Path.GetName();
				m_Path = source;
				m_Path.SetName(std::move(name));

				return *this;
			}
			
			wxString GetName() const
			{
				return m_Path.GetName();
			}
			FileItem& SetName(const wxString& name)
			{
				m_Path.SetName(name);
				return *this;
			}
			
			wxString GetFileExtension() const
			{
				return m_Path.GetExtension();
			}
			FileItem& SetFileExtension(const wxString& ext)
			{
				m_Path.SetExtension(ext);
				return *this;
			}

			// Size
			BinarySize GetSize() const
			{
				return m_Size;
			}
			FileItem& SetSize(BinarySize size)
			{
				m_Size = size;
				return *this;
			}

			double GetCompressionRatio() const
			{
				if (IsCompressed() && m_CompressedSize)
				{
					return m_Size ? m_CompressedSize.GetBytes<double>() / m_Size.GetBytes<double>() : -1;
				}
				return 1;
			}
			BinarySize GetCompressedSize() const
			{
				return m_CompressedSize;
			}
			FileItem& SetCompressedSize(BinarySize size)
			{
				m_CompressedSize = size;
				return *this;
			}

		public:
			FileItem& operator=(FileItem&&) = default;
			FileItem& operator=(const FileItem&) = default;

			explicit operator bool() const
			{
				return IsValid();
			}
			bool operator!() const
			{
				return !IsValid();
			}
	};
}
