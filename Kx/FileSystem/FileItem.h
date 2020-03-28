#pragma once
#include "Common.h"
#include "FSPath.h"
#include "Kx/General/ExtraData.h"
#include "Kx/General/BinarySize.h"
#include "Kx/General/UndefWindows.h"

namespace KxFramework
{
	class KX_API FileItem final: public ExtraDataContainer
	{
		private:
			FSPath m_Path;
			wxDateTime m_CreationTime;
			wxDateTime m_LastAccessTime;
			wxDateTime m_ModificationTime;
			BinarySize m_Size;
			BinarySize m_CompressedSize;
			FileAttribute m_Attributes = FileAttribute::None;
			ReparsePointTag m_ReparsePointTags = ReparsePointTag::None;

		protected:
			bool DoUpdateInfo();
			bool DoIsValid() const
			{
				return m_Path && m_Attributes != FileAttribute::Invalid;
			}

		public:
			FileItem() = default;
			FileItem(const FSPath& fullPath)
				:m_Path(fullPath)
			{
				DoUpdateInfo();
			}
			FileItem(const FSPath& source, const FSPath& fileName)
				:m_Path(source)
			{
				m_Path.Append(fileName);
				DoUpdateInfo();
			}
			FileItem(const FileItem&) = default;
			FileItem(FileItem&&) = default;

		public:
			// General
			bool UpdateInfo()
			{
				return DoUpdateInfo();
			}
			bool IsNormalItem() const
			{
				return DoIsValid() && !IsReparsePoint() && !IsCurrentOrParent();
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
			void SetAttributes(FileAttribute attributes)
			{
				m_Attributes = attributes;
			}
			
			ReparsePointTag GetReparsePointTags() const
			{
				return m_ReparsePointTags;
			}
			void SetReparsePointTags(ReparsePointTag tags)
			{
				m_ReparsePointTags = tags;
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

			// Path and name
			FSPath GetFullPath() const
			{
				return m_Path;
			}
			void SetFullPath(const FSPath& fullPath)
			{
				m_Path = fullPath;
			}

			FSPath GetSource() const
			{
				return m_Path.GetParent();
			}
			void SetSource(const FSPath& source)
			{
				wxString name = m_Path.GetName();

				m_Path = source;
				m_Path.SetName(std::move(name));
			}
			
			wxString GetName() const
			{
				return m_Path.GetName();
			}
			void SetName(const wxString& name)
			{
				m_Path.SetName(name);
			}
			
			wxString GetFileExtension() const
			{
				return m_Path.GetExtension();
			}
			void SetFileExtension(const wxString& ext)
			{
				m_Path.SetExtension(ext);
			}

			// Size
			BinarySize GetSize() const
			{
				return m_Size;
			}
			void SetSize(BinarySize size)
			{
				m_Size = size;
			}

			BinarySize GetCompressedSize() const
			{
				return m_CompressedSize;
			}
			void SetCompressedSize(BinarySize size)
			{
				m_CompressedSize = size;
			}
			double GetCompressionRatio() const
			{
				if (IsCompressed() && m_CompressedSize)
				{
					return m_Size ? m_CompressedSize.GetBytes<double>() / m_Size.GetBytes<double>() : -1;
				}
				return 1;
			}

		public:
			FileItem& operator=(const FileItem&) = default;
			FileItem& operator=(FileItem&&) = default;

			explicit operator bool() const
			{
				return DoIsValid();
			}
			bool operator!() const
			{
				return !DoIsValid();
			}
	};
}
