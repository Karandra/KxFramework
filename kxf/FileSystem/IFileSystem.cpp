#include "stdafx.h"
#include "IFileSystem.h"
#include "kxf/IO/IStream.h"

namespace
{
	using namespace kxf;

	template<class TStream, class TFileSystem, class TID>
	std::unique_ptr<TStream> QueryStream(TFileSystem& fs, const TID& id, FlagSet<IOStreamAccess> access, IOStreamDisposition disposition, FlagSet<IOStreamShare> share)
	{
		if (auto anyStream = fs.GetStream(id, access, disposition, share))
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
	std::unique_ptr<IInputStream> IFileSystem::OpenToRead(const FSPath& path, IOStreamDisposition disposition, FlagSet<IOStreamShare> share) const
	{
		return QueryStream<IInputStream>(const_cast<IFileSystem&>(*this), path, IOStreamAccess::Read, disposition, share);
	}
	std::unique_ptr<IOutputStream> IFileSystem::OpenToWrite(const FSPath& path, IOStreamDisposition disposition, FlagSet<IOStreamShare> share)
	{
		return QueryStream<IOutputStream>(*this, path, IOStreamAccess::Write, disposition, share);
	}
}

namespace kxf
{
	std::unique_ptr<IInputStream> IFileIDSystem::OpenToRead(const UniversallyUniqueID& id, IOStreamDisposition disposition, FlagSet<IOStreamShare> share) const
	{
		return QueryStream<IInputStream>(const_cast<IFileIDSystem&>(*this), id, IOStreamAccess::Read, disposition, share);
	}
	std::unique_ptr<IOutputStream> IFileIDSystem::OpenToWrite(const UniversallyUniqueID& id, IOStreamDisposition disposition, FlagSet<IOStreamShare> share)
	{
		return QueryStream<IOutputStream>(*this, id, IOStreamAccess::Write, disposition, share);
	}
}
