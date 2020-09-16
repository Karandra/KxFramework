#pragma once
#include "Common.h"
#include "kxf/General/DateTime.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf
{
	enum class FileAttribute: uint32_t;
}

namespace kxf
{
	class KX_API INativeStream: public RTTI::Interface<INativeStream>
	{
		KxRTTI_DeclareIID(INativeStream, {0x917d1d69, 0x16a3, 0x494d, {0x91, 0x83, 0x35, 0x6c, 0x4d, 0x39, 0x32, 0x7d}});

		public:
			INativeStream() noexcept = default;
			virtual ~INativeStream() = default;

		public:
			virtual void* GetHandle() const = 0;
			virtual bool AttachHandle(void* handle) = 0;
			virtual bool ReopenHandle(FlagSet<IOStreamAccess> access,
									  FlagSet<IOStreamShare> share,
									  FlagSet<IOStreamFlag> flags = IOStreamFlag::None
			) = 0;
			virtual void* DetachHandle() = 0;

			virtual FlagSet<FileAttribute> GetAttributes() const = 0;
			virtual bool SetAttributes(FlagSet<FileAttribute> attributes) = 0;

			virtual bool GetTimestamp(DateTime& creationTime, DateTime& modificationTime, DateTime& lastAccessTime) const = 0;
			virtual bool ChangeTimestamp(DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) = 0;
	};
}
