#include "KxStdAfx.h"
#include "KxFramework/KxArchive.h"
#include "KxFramework/KxFileItem.h"
#include "KxFramework/KxArchiveFileFinder.h"
#include "KxFramework/KxComparator.h"
#include <KxFramework/KxFile.h>
#include <KxFramework/KxFileStream.h>
#include <KxFramework/KxStreamDelegate.h>

namespace KxArchive
{
	class ExtractorCallbackBase: public KxRTTI::ImplementInterface<ExtractorCallbackBase, IExtractionCallback>
	{
		protected:
			IArchiveItems& m_ArchiveItems;

		public:
			ExtractorCallbackBase(IArchiveExtraction& archive)
				:m_ArchiveItems(*archive.QueryInterface<IArchiveItems>())
			{
			}
	};

	class FileExtractionCallback: public ExtractorCallbackBase
	{
		private:
			wxString m_Directory;
			KxFileItem m_FileItem;
			KxFileStream m_Stream;

		public:
			FileExtractionCallback(IArchiveExtraction& archive, const wxString& directory = {})
				:ExtractorCallbackBase(archive), m_Directory(directory)
			{
				if (!m_Directory.IsEmpty() && m_Directory.Last() == wxS('\\'))
				{
					m_Directory.RemoveLast(1);
				}
			}

		public:
			KxDelegateOutputStream OnGetStream(FileIndex fileIndex) override
			{
				m_FileItem = m_ArchiveItems.GetItem(fileIndex);
				if (m_FileItem.IsOK())
				{
					// Get target path
					wxString targetPath = GetTargetPath(m_FileItem);
					if (m_FileItem.IsDirectory())
					{
						// Creating a directory here supports having empty directories
						KxFile(targetPath).CreateFolder();
						return nullptr;
					}
					else
					{
						KxFile(targetPath.BeforeLast(wxS('\\'))).CreateFolder();

						m_Stream.Open(targetPath, KxFileStream::Access::Write, KxFileStream::Disposition::CreateAlways, KxFileStream::Share::Read);
						return m_Stream;
					}
				}
				return nullptr;
			}
			bool OnOperationCompleted(FileIndex fileIndex, wxOutputStream& stream) override
			{
				if (m_FileItem.IsOK())
				{
					m_Stream.SetAttributes(m_FileItem.GetAttributes());
					m_Stream.SetFileTime(m_FileItem.GetCreationTime(), m_FileItem.GetModificationTime(), m_FileItem.GetLastAccessTime());
					m_Stream.Close();

					return true;
				}
				return false;
			}
			
			virtual wxString GetTargetPath(const KxFileItem& fileItem) const
			{
				return m_Directory + wxS('\\') + fileItem.GetFullPath();
			}
	};
	class SingleFileExtractionCallback: public FileExtractionCallback
	{
		private:
			wxString m_TargetPath;

		public:
			SingleFileExtractionCallback(IArchiveExtraction& archive, const wxString& targetPath)
				:FileExtractionCallback(archive), m_TargetPath(targetPath)
			{
			}

		public:
			wxString GetTargetPath(const KxFileItem& fileItem) const override
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
			KxDelegateOutputStream OnGetStream(FileIndex fileIndex) override
			{
				KxFileItem fileItem = m_ArchiveItems.GetItem(fileIndex);
				if (fileItem.IsOK() && !fileItem.IsDirectory())
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

namespace KxArchive
{
	double IArchive::GetCompressionRatio() const
	{
		int64_t originalSize = GetOriginalSize();
		int64_t compressedSize = GetCompressedSize();

		if (originalSize > 0)
		{
			return (double)compressedSize / originalSize;
		}
		return -1;
	}
}

namespace KxArchive
{
	bool IArchiveSearch::FindFile(const wxString& searchQuery, KxFileItem& fileItem) const
	{
		FileFinder finder(*this, searchQuery);
		fileItem = finder.FindNext();

		return fileItem.IsOK();
	}
	bool IArchiveSearch::FindFileInFolder(const wxString& folder, const wxString& filter, KxFileItem& fileItem) const
	{
		FileFinder finder(*this, folder, filter);

		for (KxFileItem item = finder.FindNext(); item.IsOK(); item = finder.FindNext())
		{
			if (KxComparator::IsEqual(folder, item.GetSource(), true))
			{
				fileItem = std::move(item);
				return true;
			}
		}
		return false;
	}
}

namespace KxArchive
{
	bool IArchiveExtraction::ExtractToDirectory(const wxString& directory) const
	{
		FileExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), directory);
		return Extract(callback);
	}
	bool IArchiveExtraction::ExtractToDirectory(const wxString& directory, FileIndexView files) const
	{
		FileExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), directory);
		return Extract(callback, files);
	}

	bool IArchiveExtraction::ExtractToStream(FileIndex fileIndex, wxOutputStream& stream) const
	{
		SingleStreamExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), stream);
		return Extract(callback, fileIndex);
	}
	bool IArchiveExtraction::ExtractToFile(FileIndex fileIndex, const wxString& targetPath) const
	{
		SingleFileExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), targetPath);
		return Extract(callback, fileIndex);
	}
}
