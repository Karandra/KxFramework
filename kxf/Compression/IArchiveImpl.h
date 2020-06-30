#pragma once
#include "Common.h"
#include "IArchive.h"
#include "kxf/FileSystem/IFileSystem.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/FileSystem/FileStream.h"

namespace kxf::Compression
{
	class ExtractorCallbackBase: public RTTI::ImplementInterface<ExtractorCallbackBase, IExtractCallback>
	{
		protected:
			IArchiveExtract& m_ArchiveExtraction;
			IFileIDSystem& m_ArchiveFS;

		public:
			ExtractorCallbackBase(IArchiveExtract& archive)
				:m_ArchiveExtraction(archive), m_ArchiveFS(*archive.QueryInterface<IFileIDSystem>())
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
			FileExtractionCallback(IArchiveExtract& archive, const FSPath& directory = {})
				:ExtractorCallbackBase(archive), m_Directory(directory)
			{
			}

		public:
			OutputStreamDelegate OnGetStream(size_t fileIndex) override
			{
				m_FileItem = m_ArchiveFS.GetItem(LocallyUniqueID(fileIndex));
				if (m_FileItem)
				{
					// Get target path
					FSPath targetPath = GetTargetPath(m_FileItem);
					if (m_FileItem.IsDirectory())
					{
						// Creating a directory here supports having empty directories
						NativeFileSystem().CreateDirectory(targetPath);
						return nullptr;
					}
					else
					{
						NativeFileSystem().CreateDirectory(targetPath.GetParent());
						m_Stream.Open(targetPath, FileStreamAccess::Write, FileStreamDisposition::CreateAlways, FileStreamShare::Read);
						return m_Stream;
					}
				}
				return nullptr;
			}
			bool OnOperationCompleted(size_t fileIndex, wxOutputStream& stream) override
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
			SingleFileExtractionCallback(IArchiveExtract& archive, const FSPath& targetPath)
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
			SingleStreamExtractionCallback(IArchiveExtract& archive, wxOutputStream& stream)
				:ExtractorCallbackBase(archive), m_Stream(stream)
			{
			}

		public:
			OutputStreamDelegate OnGetStream(size_t fileIndex) override
			{
				FileItem fileItem = m_ArchiveFS.GetItem(LocallyUniqueID(fileIndex));
				if (fileItem && !fileItem.IsDirectory())
				{
					return m_Stream;
				}
				return nullptr;
			}
			bool OnOperationCompleted(size_t fileIndex, wxOutputStream& stream) override
			{
				return true;
			}
	};
}
