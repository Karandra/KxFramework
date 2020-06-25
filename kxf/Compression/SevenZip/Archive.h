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
	class Archive: public Private::WithEvtHandler, public RTTI::ImplementInterface<Archive, IArchive, IArchiveProperties>
	{
		protected:
			struct Data final
			{
				FSPath FilePath;
				COMPtr<IStream> ArchiveStream;
				COMPtr<IInArchive> ArchiveStreamReader;
				COMPtr<Private::InStreamWrapper> ArchiveStreamWrapper;

				// Metadata
				size_t ItemCount = 0;
				bool IsLoaded = false;
				bool OverrideCompressionFormat = false;

				mutable int64_t OriginalSize = -1;
				mutable int64_t CompressedSize = -1;

				struct
				{
					CompressionFormat CompressionFormat = CompressionFormat::Unknown;
					CompressionMethod CompressionMethod = CompressionMethod::LZMA;
					CompressionLevel CompressionLevel = CompressionLevel::Normal;
					int DictionarySize = 5;
					bool Solid = false;
					bool MultiThreaded = true;
				} Properties;
			} m_Data;

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
				Open(std::move(filePath));
			}
			Archive(const Archive&) = delete;
			Archive(Archive&& other)
			{
				*this = std::move(other);
			}
			virtual ~Archive();

		public:
			// IArchive
			FSPath GetFilePath() const override
			{
				return m_Data.FilePath;
			}
			bool IsOpened() const noexcept override
			{
				return m_Data.IsLoaded;
			}
			bool Open(const FSPath& filePath) override;
			void Close() override;

			size_t GetItemCount() const override
			{
				return m_Data.ItemCount;
			}
			BinarySize GetOriginalSize() const override;
			BinarySize GetCompressedSize() const override;

			FileItem GetItem(size_t index) const;

		public:
			// IArchiveProperties
			std::optional<bool> GetPropertyBool(StringView property) const override
			{
				if (property == Compression::Property::Compression_Solid)
				{
					return m_Data.Properties.Solid;
				}
				if (property == Compression::Property::Compression_MultiThreaded)
				{
					return m_Data.Properties.MultiThreaded;
				}
				return {};
			}
			bool SetPropertyBool(StringView property, bool value) override
			{
				if (property == Compression::Property::Compression_Solid)
				{
					m_Data.Properties.Solid = value;
					return true;
				}
				if (property == Compression::Property::Compression_MultiThreaded)
				{
					m_Data.Properties.MultiThreaded = true;
					return true;
				}
				return false;
			}

			std::optional<int64_t> GetPropertyInt(StringView property) const override
			{
				if (property == Compression::Property::Common_ItemCount)
				{
					return m_Data.ItemCount;
				}
				if (property == Compression::Property::Common_OriginalSize)
				{
					return GetOriginalSize().GetBytes();
				}
				if (property == Compression::Property::Common_CompressedSize)
				{
					return GetCompressedSize().GetBytes();
				}
				if (property == Compression::Property::Compression_Format)
				{
					return static_cast<int>(m_Data.Properties.CompressionFormat);
				}
				if (property == Compression::Property::Compression_Method)
				{
					return static_cast<int>(m_Data.Properties.CompressionMethod);
				}
				if (property == Compression::Property::Compression_Level)
				{
					return static_cast<int>(m_Data.Properties.CompressionLevel);
				}
				if (property == Compression::Property::Compression_DictionarySize)
				{
					return m_Data.Properties.DictionarySize;
				}
				return {};
			}
			bool SetPropertyInt(StringView property, int64_t value) override
			{
				if (property == Compression::Property::Compression_Format)
				{
					m_Data.Properties.CompressionFormat = static_cast<CompressionFormat>(value);
					return true;
				}
				if (property == Compression::Property::Compression_Method)
				{
					m_Data.Properties.CompressionMethod = static_cast<CompressionMethod>(value);
					return true;
				}
				if (property == Compression::Property::Compression_Level)
				{
					m_Data.Properties.CompressionLevel = static_cast<CompressionLevel>(value);
					return true;
				}
				if (property == Compression::Property::Compression_DictionarySize)
				{
					m_Data.Properties.DictionarySize = value;
					return true;
				}
				return false;
			}

			std::optional<double> GetPropertyFloat(StringView property) const override
			{
				return {};
			}
			bool SetPropertyFloat(StringView property, double value) override
			{
				return false;
			}

			std::optional<String> GetPropertyString(StringView property) const override
			{
				if (property == Compression::Property::Common_FilePath)
				{
					return m_Data.FilePath;
				}
				return {};
			}
			bool SetPropertyString(StringView property, StringView value)
			{
				return false;
			}

		public:
			// IArchiveExtraction
			// Extract files using provided extractor
			bool Extract(COMPtr<Private::Callback::ExtractArchive> extractor) const;
			bool Extract(COMPtr<Private::Callback::ExtractArchive> extractor, Compression::FileIndexView files) const;

			// Extract entire archive or only specified files into a directory
			bool ExtractToDirectory(const FSPath& directory) const;
			bool ExtractToDirectory(const FSPath& directory, Compression::FileIndexView files) const;
	
		public:
			// IArchiveCompression
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
			// IFileSystem

		public:
			// IFileIDSystem
			
		public:
			Archive& operator=(const Archive&) = delete;
			Archive& operator=(Archive&& other);

			explicit operator bool() const
			{
				return IsOpened();
			}
			bool operator!() const
			{
				return !IsOpened();
			}
	};
}
