#pragma once
#include "Common.h"
#include "FSPath.h"
#include "StorageVolume.h"
#include "kxf/General/BinarySize.h"
#include "kxf/General/LocallyUniqueID.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/RTTI/QueryInterface.h"
#include "kxf/Utility/Common.h"

namespace kxf
{
	class FileItem;

	class IStream;
	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	class KX_API IFileSystem: public RTTI::Interface<IFileSystem>
	{
		KxRTTI_DeclareIID(IFileSystem, {0x950f74ec, 0x6352, 0x47a1, {0xac, 0x8f, 0x43, 0x6a, 0x6, 0x1e, 0x9f, 0x65}});

		public:
			using TEnumItemsFunc = std::function<bool(FileItem)>;
			using TCopyItemFunc = std::function<bool(BinarySize, BinarySize)>;

		public:
			virtual ~IFileSystem() = default;

		public:
			virtual bool IsNull() const = 0;

			virtual bool IsValidPathName(const FSPath& path) const = 0;
			virtual String GetForbiddenPathNameCharacters(const String& except = {}) const = 0;

			virtual FSPath GetCurrentDirectory() const = 0;
			virtual FSPath ResolvePath(const FSPath& relativePath) const = 0;

			virtual bool ItemExist(const FSPath& path) const = 0;
			virtual bool FileExist(const FSPath& path) const = 0;
			virtual bool DirectoryExist(const FSPath& path) const = 0;

			virtual FileItem GetItem(const FSPath& path) const = 0;
			virtual size_t EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPath& query = {}, FlagSet<FSActionFlag> flags = {}) const = 0;
			virtual bool IsDirectoryEmpty(const FSPath& directory) const = 0;
			
			virtual bool CreateDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes) = 0;
			virtual bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) = 0;

			virtual bool CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool RemoveItem(const FSPath& path) = 0;
			virtual bool RemoveDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) = 0;

			virtual std::unique_ptr<IStream> GetStream(const FSPath& path,
													   FlagSet<IOStreamAccess> access,
													   IOStreamDisposition disposition,
													   FlagSet<IOStreamShare> share = IOStreamShare::Read,
													   FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
													   FlagSet<FSActionFlag> flags = {}
			) = 0;
			std::unique_ptr<IInputStream> OpenToRead(const FSPath& path,
													 IOStreamDisposition disposition = IOStreamDisposition::OpenExisting,
													 FlagSet<IOStreamShare> share = IOStreamShare::Read,
													 FlagSet<FSActionFlag> flags = {}
			) const;
			std::unique_ptr<IOutputStream> OpenToWrite(const FSPath& path,
													   IOStreamDisposition disposition = IOStreamDisposition::CreateAlways,
													   FlagSet<IOStreamShare> share = IOStreamShare::Read,
													   FlagSet<FSActionFlag> flags = {}
			);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};

	class KX_API IFileIDSystem: public RTTI::Interface<IFileIDSystem>
	{
		KxRTTI_DeclareIID(IFileIDSystem, {0x8a4f7e63, 0x6092, 0x4859, {0xa1, 0x74, 0x25, 0x8, 0x7a, 0x4a, 0x90, 0xcb}});

		public:
			virtual ~IFileIDSystem() = default;

		public:
			virtual bool IsNull() const = 0;
			virtual UniversallyUniqueID GetLookupScope() const = 0;

			virtual bool ItemExist(const UniversallyUniqueID& id) const = 0;
			virtual bool FileExist(const UniversallyUniqueID& id) const = 0;
			virtual bool DirectoryExist(const UniversallyUniqueID& id) const = 0;

			virtual FileItem GetItem(const UniversallyUniqueID& id) const = 0;
			virtual size_t EnumItems(const UniversallyUniqueID& id, IFileSystem::TEnumItemsFunc func, FlagSet<FSActionFlag> flags = {}) const = 0;
			virtual bool IsDirectoryEmpty(const UniversallyUniqueID& id) const = 0;

			virtual bool ChangeAttributes(const UniversallyUniqueID& id, FlagSet<FileAttribute> attributes) = 0;
			virtual bool ChangeTimestamp(const UniversallyUniqueID& id, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) = 0;

			virtual bool CopyItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, IFileSystem::TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool MoveItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, IFileSystem::TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) = 0;
			virtual bool RemoveItem(const UniversallyUniqueID& id) = 0;
			virtual bool RemoveDirectory(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags = {}) = 0;

			virtual std::unique_ptr<IStream> GetStream(const UniversallyUniqueID& id,
													   FlagSet<IOStreamAccess> access,
													   IOStreamDisposition disposition,
													   FlagSet<IOStreamShare> share = IOStreamShare::Read,
													   FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
													   FlagSet<FSActionFlag> flags = {}
			) = 0;
			std::unique_ptr<IInputStream> OpenToRead(const UniversallyUniqueID& id,
													 IOStreamDisposition disposition = IOStreamDisposition::OpenExisting,
													 FlagSet<IOStreamShare> share = IOStreamShare::Read,
													 FlagSet<FSActionFlag> flags = {}
			) const;
			std::unique_ptr<IOutputStream> OpenToWrite(const UniversallyUniqueID& id,
													   IOStreamDisposition disposition = IOStreamDisposition::CreateAlways,
													   FlagSet<IOStreamShare> share = IOStreamShare::Read,
													   FlagSet<FSActionFlag> flags = {}
			);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}

namespace kxf::FileSystem
{
	IFileSystem& GetNullFileSystem() noexcept;
	IFileIDSystem& GetNullFileIDSystem() noexcept;
}
