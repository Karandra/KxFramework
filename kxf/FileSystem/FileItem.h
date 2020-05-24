#pragma once
#include "Common.h"
#include "FSPath.h"
#include "kxf/General/String.h"
#include "kxf/General/ExtraData.h"
#include "kxf/General/BinarySize.h"
#include <wx/datetime.h>

namespace kxf
{
	class IFileSystem;
}

namespace kxf
{
	class KX_API FileItem final: public TrivialExtraDataContainer
	{
		private:
			FSPath m_Path;
			BinarySize m_Size;
			BinarySize m_CompressedSize;
			DateTime m_CreationTime;
			DateTime m_LastAccessTime;
			DateTime m_ModificationTime;
			FlagSet<FileAttribute> m_Attributes;
			FlagSet<ReparsePointTag> m_ReparsePointTags;

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
					const String name = m_Path.GetName();
					return name == wxS("..") || name == wxS('.');
				}
				return false;
			}

			// Attributes
			FlagSet<FileAttribute> GetAttributes() const
			{
				return m_Attributes;
			}
			FileItem& SetAttributes(FlagSet<FileAttribute> attributes)
			{
				m_Attributes = attributes;
				return *this;
			}
			
			FlagSet<ReparsePointTag> GetReparsePointTags() const
			{
				return m_ReparsePointTags;
			}
			FileItem& SetReparsePointTags(FlagSet<ReparsePointTag> tags)
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
			DateTime GetCreationTime() const
			{
				return m_CreationTime;
			}
			FileItem& SetCreationTime(DateTime value)
			{
				m_CreationTime = value;
				return *this;
			}
			
			DateTime GetLastAccessTime() const
			{
				return m_LastAccessTime;
			}
			FileItem& SetLastAccessTime(DateTime value)
			{
				m_LastAccessTime = value;
				return *this;
			}
			
			DateTime GetModificationTime() const
			{
				return m_ModificationTime;
			}
			FileItem& SetModificationTime(DateTime value)
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
				String name = m_Path.GetName();
				m_Path = source;
				m_Path.SetName(std::move(name));

				return *this;
			}
			
			String GetName() const
			{
				return m_Path.GetName();
			}
			FileItem& SetName(const String& name)
			{
				m_Path.SetName(name);
				return *this;
			}
			
			String GetFileExtension() const
			{
				return m_Path.GetExtension();
			}
			FileItem& SetFileExtension(const String& ext)
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
					return GetSizeRatio(m_CompressedSize, m_Size);
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
