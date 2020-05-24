#pragma once
#include "Common.h"
#include "IArchive.h"
#include "kxf/FileSystem/FileStream.h"
#include "kxf/FileSystem/NativeFileSystem.h"

namespace kxf::Compression
{
	class ExtractorCallbackBase: public RTTI::ImplementInterface<ExtractorCallbackBase, IExtractionCallback>
	{
		protected:
			IArchiveItems& m_ArchiveItems;
			IArchiveExtraction& m_ArchiveExtraction;

		public:
			ExtractorCallbackBase(IArchiveExtraction& archive)
				:m_ArchiveItems(*archive.QueryInterface<IArchiveItems>()), m_ArchiveExtraction(archive)
			{
			}
	};
}

namespace kxf::Compression
{
	class FileExtractionCallback: public ExtractorCallbackBase
	{
		private:
			FSPath m_Directory;
			FileItem m_FileItem;
			FileStream m_Stream;

		public:
			FileExtractionCallback(IArchiveExtraction& archive, const FSPath& directory = {})
				:ExtractorCallbackBase(archive), m_Directory(directory)
			{
			}

		public:
			OutputStreamDelegate OnGetStream(FileIndex fileIndex) override
			{
				m_FileItem = m_ArchiveItems.GetItem(fileIndex);
				if (m_FileItem)
				{
					// Get target path
					FSPath targetPath = GetTargetPath(m_FileItem);
					if (m_FileItem.IsDirectory())
					{
						// Creating a directory here supports having empty directories
						NativeFileSystem::Get().CreateDirectory(targetPath);
						return nullptr;
					}
					else
					{
						NativeFileSystem::Get().CreateDirectory(targetPath.GetParent());
						m_Stream.Open(targetPath, FileStreamAccess::Write, FileStreamDisposition::CreateAlways, FileStreamShare::Read);
						return m_Stream;
					}
				}
				return nullptr;
			}
			bool OnOperationCompleted(FileIndex fileIndex, wxOutputStream& stream) override
			{
				if (m_FileItem)
				{
					m_Stream.SetAttributes(m_FileItem.GetAttributes());
					m_Stream.ChangeTimestamp(m_FileItem.GetCreationTime(), m_FileItem.GetModificationTime(), m_FileItem.GetLastAccessTime());
					m_Stream.Close();

					return true;
				}
				return false;
			}
			
			virtual FSPath GetTargetPath(const FileItem& fileItem) const
			{
				return m_Directory / fileItem.GetFullPath();
			}
	};

	class SingleFileExtractionCallback: public FileExtractionCallback
	{
		private:
			FSPath m_TargetPath;

		public:
			SingleFileExtractionCallback(IArchiveExtraction& archive, const FSPath& targetPath)
				:FileExtractionCallback(archive), m_TargetPath(targetPath)
			{
			}

		public:
			FSPath GetTargetPath(const FileItem& fileItem) const override
			{
				return m_TargetPath;
			}
	};

	class SingleStreamExtractionCallback: public ExtractorCallbackBase
	{
		private:
			wxOutputStream& m_Stream;

		public:
			SingleStreamExtractionCallback(IArchiveExtraction& archive, wxOutputStream& stream)
				:ExtractorCallbackBase(archive), m_Stream(stream)
			{
			}

		public:
			OutputStreamDelegate OnGetStream(FileIndex fileIndex) override
			{
				FileItem fileItem = m_ArchiveItems.GetItem(fileIndex);
				if (fileItem && !fileItem.IsDirectory())
				{
					return m_Stream;
				}
				return nullptr;
			}
			bool OnOperationCompleted(FileIndex fileIndex, wxOutputStream& stream) override
			{
				return true;
			}
	};
}
