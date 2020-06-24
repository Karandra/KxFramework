#pragma once
#include "Common.h"
#include "Private/WithEvtHandler.h"
#include "kxf/System/COM.h"
#include "kxf/Compression/IArchive.h"
struct IStream;
struct IInArchive;

namespace kxf::SevenZip::Private
{
	class InStreamWrapper;

	namespace Callback
	{
		class OpenArchive;
		class UpdateArchive;
		class ExtractArchive;
	}
}

namespace kxf::SevenZip
{
	class Archive: public Private::WithEvtHandler
	{
		protected:
			FSPath m_ArchivePath;
			COMPtr<IStream> m_ArchiveStream;
			COMPtr<IInArchive> m_ArchiveStreamReader;
			COMPtr<Private::InStreamWrapper> m_ArchiveStreamWrapper;

			// Metadata
			size_t m_ItemCount = 0;
			bool m_IsLoaded = false;
			bool m_OverrideCompressionFormat = false;

			// Properties
			CompressionFormat m_Property_CompressionFormat = CompressionFormat::Unknown;
			CompressionMethod m_Property_CompressionMethod = CompressionMethod::LZMA;
			CompressionLevel m_Property_CompressionLevel = CompressionLevel::Normal;
			int m_Property_DictionarySize = 5;
			bool m_Property_Solid = false;
			bool m_Property_MultiThreaded = true;

		private:
			void InvalidateCache();
			bool InitCompressionFormat();
			bool InitMetadata();
			bool InitArchiveStreams();
			void RewindArchiveStreams();

		protected:
			bool DoExtract(COMPtr<Private::Callback::ExtractArchive> extractor, Compression::FileIndexView* files) const;
			bool DoCompress(const FSPath& pathPrefix, const std::vector<FileItem>& filePaths, const std::vector<FSPath>& inArchiveFilePaths);
			bool FindAndCompressFiles(const IFileSystem& fileSystem, const FSPath& directory, const FSPathQuery& searchPattern, const FSPath& pathPrefix, bool recursion);

		public:
			Archive(wxEvtHandler* evtHandler = nullptr);
			Archive(FSPath filePath, wxEvtHandler* evtHandler = nullptr)
				:Archive(evtHandler)
			{
				Load(std::move(filePath));
			}
			Archive(const Archive&) = delete;
			Archive(Archive&& other)
			{
				*this = std::move(other);
			}
			virtual ~Archive();

		public:
			bool Load(FSPath filePath);
			bool IsLoaded() const noexcept
			{
				return m_IsLoaded;
			}

			size_t GetItemCount() const
			{
				return m_ItemCount;
			}
			FileItem GetItem(size_t index) const;

			int64_t GetOriginalSize() const;
			int64_t GetCompressedSize() const;

		public:
			FSPath GetProperty_FilePath() const
			{
				return m_ArchivePath;
			}

			CompressionFormat GetProperty_CompressionFormat() const
			{
				return m_Property_CompressionFormat;
			}
			void SetProperty_CompressionFormat(CompressionFormat format)
			{
				m_OverrideCompressionFormat = true;
				InvalidateCache();
				m_Property_CompressionFormat = format;
			}
			
			int GetProperty_CompressionLevel() const
			{
				return static_cast<int>(m_Property_CompressionLevel);
			}
			void SetProperty_CompressionLevel(int value)
			{
				m_Property_CompressionLevel = static_cast<CompressionLevel>(value);
			}
			void SetProperty_CompressionLevel(CompressionLevel value)
			{
				m_Property_CompressionLevel = value;
			}
			
			int GetProperty_DictionarySize() const
			{
				return m_Property_DictionarySize;
			}
			void SetProperty_DictionarySize(int value)
			{
				m_Property_DictionarySize = value;
			}

			CompressionMethod GetProperty_CompressionMethod() const
			{
				return m_Property_CompressionMethod;
			}
			void SetProperty_CompressionMethod(CompressionMethod nMethod)
			{
				m_Property_CompressionMethod = nMethod;
			}
			
			bool GetProperty_Solid() const
			{
				return m_Property_Solid;
			}
			void SetProperty_Solid(bool isSolid)
			{
				m_Property_Solid = isSolid;
			}
			
			bool GetProperty_MultiThreaded() const
			{
				return m_Property_MultiThreaded;
			}
			void SetProperty_MultiThreaded(bool isMT)
			{
				m_Property_MultiThreaded = isMT;
			}

		public:
			// Extract files using provided extractor
			bool Extract(COMPtr<Private::Callback::ExtractArchive> extractor) const;
			bool Extract(COMPtr<Private::Callback::ExtractArchive> extractor, Compression::FileIndexView files) const;

			// Extract entire archive or only specified files into a directory
			bool ExtractToDirectory(const FSPath& directory) const;
			bool ExtractToDirectory(const FSPath& directory, Compression::FileIndexView files) const;
	
		public:
			// Includes the last directory as the root in the archive, e.g. specifying "C:\Temp\MyFolder"
			// makes "MyFolder" the single root item in archive with the files within it included.
			bool CompressDirectory(const FSPath& directory, bool isRecursive = true);

			// Excludes the last directory as the root in the archive, its contents are at root instead. E.g.
			// specifying "C:\Temp\MyFolder" make the files in "MyFolder" the root items in the archive.
			bool CompressFiles(const FSPath& directory, const FSPath& searchFilter = {}, bool recursive = true);
			bool CompressSpecifiedFiles(const std::vector<FSPath>& sourceFiles, const std::vector<FSPath>& archivePaths);

			// Compress just this single file as the root item in the archive.
			bool CompressFile(const FSPath& filePath);

			// Same as above, but places compressed file into 'archivePath' folder inside the archive
			bool CompressFile(const FSPath& filePath, const FSPath& archivePath);

		public:
			Archive& operator=(const Archive&) = delete;
			Archive& operator=(Archive&& other);

			explicit operator bool() const
			{
				return IsLoaded();
			}
			bool operator!() const
			{
				return !IsLoaded();
			}
	};
}
