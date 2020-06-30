#pragma once
#include "Common.h"
#include "Private/WithEvtHandler.h"
#include "kxf/System/COM.h"
#include "kxf/Compression/IArchive.h"
struct IStream;
struct IInArchive;

namespace kxf::SevenZip::Private
{
	class InStreamWrapper_IStream;

	namespace Callback
	{
		class OpenArchive;
		class UpdateArchive;
		class ExtractArchive;
	}
}

namespace kxf::SevenZip
{
	class Archive:
		public Private::WithEvtHandler,
		public RTTI::ImplementInterface<Archive,
			IArchive,
			IArchiveProperties,
			IArchiveExtraction,
			IArchiveUpdating,
			IFileSystem,
			IFileIDSystem
		>
	{
		protected:
			struct Data final
			{
				FSPath FilePath;
				COMPtr<IStream> ArchiveStream;
				COMPtr<IInArchive> ArchiveStreamReader;
				COMPtr<Private::InStreamWrapper_IStream> ArchiveStreamWrapper;

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
			bool DoExtract(Private::Callback::ExtractArchive& extractor, Compression::FileIndexView* files) const;
			bool DoUpdate(Private::Callback::UpdateArchive& updater, size_t itemCount);

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
			FileItem GetItem(size_t index) const override;
			
			BinarySize GetOriginalSize() const override;
			BinarySize GetCompressedSize() const override;

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
			
			// Extracts files using provided callback interface
			bool Extract(Compression::IExtractionCallback& callback) const override;
			bool Extract(Compression::IExtractionCallback& callback, Compression::FileIndexView files) const override;

			// Extract entire archive or only specified files into a directory
			bool ExtractToFS(IFileSystem& fileSystem, const FSPath& directory) const override;
			bool ExtractToFS(IFileSystem& fileSystem, const FSPath& directory, Compression::FileIndexView files) const override;
			
			// Extract specified file into a stream
			bool ExtractToStream(size_t index, wxOutputStream& stream) const override;
			
		public:
			// IArchiveUpdating

			// Add files using provided callback interface
			bool Update(Compression::IUpdateCallback& callback, size_t itemCount) override;

			// Add files from the provided file system
			bool UpdateFromFS(const IFileSystem& fileSystem, const FSPath& directory, const FSPathQuery& query = {}, FlagSet<FSEnumItemsFlag> flags = {}) override;

		public:
			// IFileSystem
			bool ItemExist(const FSPath& path) const override;
			bool FileExist(const FSPath& path) const override;
			bool DirectoryExist(const FSPath& path) const override;

			FileItem GetItem(const FSPath& path) const override;
			size_t EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query = {}, FlagSet<FSEnumItemsFlag> flags = {}) const override;
			bool IsDirectoryEmpty(const FSPath& directory) const override;

			bool CreateDirectory(const FSPath& path) override
			{
				return false;
			}
			bool ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes) override
			{
				return false;
			}
			bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) override
			{
				return false;
			}

			bool CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override
			{
				return false;
			}
			bool MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override
			{
				return false;
			}
			bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSCopyItemFlag> flags = {}) override
			{
				return false;
			}
			bool RemoveItem(const FSPath& path) override
			{
				return false;
			}

			std::unique_ptr<wxInputStream> OpenToRead(const FSPath& path) const override
			{
				return nullptr;
			}
			std::unique_ptr<wxOutputStream> OpenToWrite(const FSPath& path) override
			{
				return nullptr;
			}

		public:
			// IFileIDSystem
			UniversallyUniqueID GetLookupScope() const override
			{
				return {};
			}

			bool ItemExist(const UniversallyUniqueID& id) const override;
			bool FileExist(const UniversallyUniqueID& id) const override;
			bool DirectoryExist(const UniversallyUniqueID& id) const override;

			FileItem GetItem(const UniversallyUniqueID& id) const override;
			size_t EnumItems(const UniversallyUniqueID& id, TEnumItemsFunc func, FlagSet<FSEnumItemsFlag> flags = {}) const override;
			bool IsDirectoryEmpty(const UniversallyUniqueID& id) const override;

			bool ChangeAttributes(const UniversallyUniqueID& id, FlagSet<FileAttribute> attributes) override
			{
				return false;
			}
			bool ChangeTimestamp(const UniversallyUniqueID& id, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) override
			{
				return false;
			}

			bool CopyItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, IFileSystem::TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override
			{
				return false;
			}
			bool MoveItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, IFileSystem::TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override
			{
				return false;
			}
			bool RemoveItem(const UniversallyUniqueID& id) override
			{
				return false;
			}

			std::unique_ptr<wxInputStream> OpenToRead(const UniversallyUniqueID& id) const override
			{
				return nullptr;
			}
			std::unique_ptr<wxOutputStream> OpenToWrite(const UniversallyUniqueID& id) override
			{
				return nullptr;
			}

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
