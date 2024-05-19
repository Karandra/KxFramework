#include "KxfPCH.h"
#include "IFileSystem.h"
#include "NullFileSystem.h"
#include "kxf/IO/IStream.h"

namespace
{
	using namespace kxf;

	kxf::FileSystem::NullFileSystem g_NullFileSystem;

	template<class TStream, class TFileSystem, class TID>
	std::unique_ptr<TStream> QueryStream(TFileSystem& fs, const TID& id, FlagSet<IOStreamAccess> access, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<FSActionFlag> flags)
	{
		if (auto anyStream = fs.GetStream(id, access, disposition, share, IOStreamFlag::None, flags))
		{
			if (auto desiredStream = anyStream->QueryInterface<TStream>())
			{
				anyStream.release();
				return std::unique_ptr<TStream>(desiredStream.get());
			}
		}
		return {};
	}
}

namespace kxf
{
	std::unique_ptr<IInputStream> IFileSystem::OpenToRead(const FSPath& path, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<FSActionFlag> flags) const
	{
		return QueryStream<IInputStream>(const_cast<IFileSystem&>(*this), path, IOStreamAccess::Read, disposition, share, flags);
	}
	std::unique_ptr<IOutputStream> IFileSystem::OpenToWrite(const FSPath& path, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<FSActionFlag> flags)
	{
		return QueryStream<IOutputStream>(*this, path, IOStreamAccess::Write, disposition, share, flags);
	}
}

namespace kxf
{
	std::unique_ptr<IInputStream> IFileSystemWithID::OpenToRead(const UniversallyUniqueID& id, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<FSActionFlag> flags) const
	{
		return QueryStream<IInputStream>(const_cast<IFileSystemWithID&>(*this), id, IOStreamAccess::Read, disposition, share, flags);
	}
	std::unique_ptr<IOutputStream> IFileSystemWithID::OpenToWrite(const UniversallyUniqueID& id, IOStreamDisposition disposition, FlagSet<IOStreamShare> share, FlagSet<FSActionFlag> flags)
	{
		return QueryStream<IOutputStream>(*this, id, IOStreamAccess::Write, disposition, share, flags);
	}
}


namespace kxf::FileSystem
{
	IFileSystem& GetNullFileSystem() noexcept
	{
		return g_NullFileSystem;
	}
	IFileSystemWithID& GetNullFileSystemWithID() noexcept
	{
		return g_NullFileSystem;
	}
}
