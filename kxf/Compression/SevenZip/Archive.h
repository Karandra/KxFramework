#pragma once
#include "Common.h"
#include "kxf/EventSystem/IWithEvtHandler.h"
#include "Private/WithEvtHandler.h"
#include "kxf/System/COM.h"
#include "kxf/Utility/WithOptionalOwnership.h"
struct IInArchive;

namespace kxf::SevenZip::Private::Callback
{
	class UpdateArchive;
	class ExtractArchive;
}

namespace kxf::SevenZip
{
	class Archive: public RTTI::ImplementInterface
		<
			Archive,
			IArchive,
			IArchiveProperties,
			IArchiveExtract,
			IArchiveUpdate,
			IWithEvtHandler,
			IFileSystem,
			IFileIDSystem
		>
	{
		protected:
			struct Data final
			{
				InputStreamDelegate Stream;
				COMPtr<IInArchive> InArchive;

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
			Utility::WithOptionalOwnership<wxEvtHandler> m_EvtHandler;

		private:
			void InvalidateCache();
			bool InitCompressionFormat();
			bool InitMetadata();
			bool InitArchiveStreams();
			void RewindArchiveStreams();

		protected:
			bool DoOpen(InputStreamDelegate stream);
			void DoClose();
			bool DoExtract(COMPtr<Private::Callback::ExtractArchive> extractor, Compression::FileIndexView* files) const;
			bool DoUpdate(wxOutputStream& stream, COMPtr<Private::Callback::UpdateArchive> updater, size_t itemCount);

		public:
			Archive();
			Archive(InputStreamDelegate stream)
				:Archive()
			{
				DoOpen(std::move(stream));
			}
			Archive(Archive&& other)
				:Archive()
			{
				*this = std::move(other);
			}
			Archive(const Archive&) = delete;
			~Archive();

		public:
			// IWithEvtHandler
			wxEvtHandler* GetEvtHandler() const override
			{
				return m_EvtHandler;
			}
			void SetEvtHandler(wxEvtHandler& evtHandler) override
			{
				m_EvtHandler.Assign(evtHandler);
			}
			void SetEvtHandler(std::unique_ptr<wxEvtHandler> evtHandler) override
			{
				m_EvtHandler.Assign(std::move(evtHandler));
			}

		public:
			// IArchive
			bool IsOpened() const noexcept override
			{
				return m_Data.IsLoaded;
			}
			bool Open(InputStreamDelegate stream) override
			{
				return DoOpen(std::move(stream));
			}
			void Close() override
			{
				DoClose();
			}

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
				return {};
			}
			bool SetPropertyString(StringView property, StringView value)
			{
				return false;
			}

		public:
			// IExtractCallback
			
			// Extracts files using provided callback interface
			bool Extract(Compression::IExtractCallback& callback) const override;
			bool Extract(Compression::IExtractCallback& callback, Compression::FileIndexView files) const override;

			// Extract entire archive or only specified files into a directory
			bool ExtractToFS(IFileSystem& fileSystem, const FSPath& directory) const override;
			bool ExtractToFS(IFileSystem& fileSystem, const FSPath& directory, Compression::FileIndexView files) const override;
			
			// Extract specified file into a stream
			bool ExtractToStream(size_t index, wxOutputStream& stream) const override;
			
		public:
			// IArchiveUpdate
			
			// Add files using provided callback interface
			bool Update(wxOutputStream& stream, Compression::IUpdateCallback& callback, size_t itemCount) override;

			// Add files from the provided file system
			bool UpdateFromFS(wxOutputStream& stream, const IFileSystem& fileSystem, const FSPath& directory, const FSPathQuery& query = {}, FlagSet<FSActionFlag> flags = {}) override;

		public:
			// IFileSystem
			FSPath GetCurrentDirectory() const override
			{
				return {};
			}
			FSPath ResolvePath(const FSPath& relativePath) const override
			{
				return {};
			}

			bool ItemExist(const FSPath& path) const override;
			bool FileExist(const FSPath& path) const override;
			bool DirectoryExist(const FSPath& path) const override;

			FileItem GetItem(const FSPath& path) const override;
			size_t EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query = {}, FlagSet<FSActionFlag> flags = {}) const override;
			bool IsDirectoryEmpty(const FSPath& directory) const override;

			bool CreateDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) override
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

			bool CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool RemoveItem(const FSPath& path) override
			{
				return false;
			}
			bool RemoveDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) override
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
			size_t EnumItems(const UniversallyUniqueID& id, TEnumItemsFunc func, FlagSet<FSActionFlag> flags = {}) const override;
			bool IsDirectoryEmpty(const UniversallyUniqueID& id) const override;

			bool ChangeAttributes(const UniversallyUniqueID& id, FlagSet<FileAttribute> attributes) override
			{
				return false;
			}
			bool ChangeTimestamp(const UniversallyUniqueID& id, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) override
			{
				return false;
			}

			bool CopyItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, IFileSystem::TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool MoveItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, IFileSystem::TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool RemoveItem(const UniversallyUniqueID& id) override
			{
				return false;
			}
			bool RemoveDirectory(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags = {}) override
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
			Archive& operator=(Archive&& other);
			Archive& operator=(const Archive&) = delete;

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
