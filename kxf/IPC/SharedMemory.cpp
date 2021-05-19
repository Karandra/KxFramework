#include "KxfPCH.h"
#include "SharedMemory.h"
#include <Windows.h>
#include "kxf/IO/MemoryStream.h"
#include "kxf/System/UndefWindows.h"
#include "kxf/Utility/Memory.h"

namespace
{
	using namespace kxf;

	constexpr FlagSet<uint32_t> ConvertProtection(FlagSet<MemoryProtection> protection) noexcept
	{
		if (protection & MemoryProtection::Execute)
		{
			if (protection & MemoryProtection::RW)
			{
				return PAGE_EXECUTE_READWRITE;
			}
			if (protection & MemoryProtection::Read)
			{
				return PAGE_EXECUTE_READ;
			}
		}
		else
		{
			if (protection & MemoryProtection::RW)
			{
				return PAGE_READWRITE;
			}
			if (protection & MemoryProtection::Read)
			{
				return PAGE_READONLY;
			}
		}
		return {};
	}
	constexpr FlagSet<uint32_t> ConvertMemoryAccess(FlagSet<MemoryProtection> protection) noexcept
	{
		FlagSet<uint32_t> value;
		if (protection & MemoryProtection::RW)
		{
			value = FILE_MAP_ALL_ACCESS;
		}
		else if (protection & MemoryProtection::Write)
		{
			value = FILE_MAP_WRITE;
		}
		else if (protection & MemoryProtection::Read)
		{
			value = FILE_MAP_READ;
		}

		value.Add(FILE_MAP_EXECUTE, protection & MemoryProtection::Execute);
		value.Add(FILE_MAP_COPY, protection & MemoryProtection::CopyOnWrite);
		return value;
	}
}

namespace kxf::IPC
{
	void* AllocateSharedMemoryRegion(void*& buffer, size_t size, FlagSet<MemoryProtection> protection, const wchar_t* name) noexcept
	{
		ULARGE_INTEGER sizeULI = {};
		sizeULI.QuadPart = size;

		const auto protectionWin32 = ConvertProtection(protection);
		const auto memoryAccessWin32 = ConvertMemoryAccess(protection);

		void* handle = ::CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, *protectionWin32, sizeULI.HighPart, sizeULI.LowPart, name);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			buffer = ::MapViewOfFile(handle, *memoryAccessWin32, 0, 0, size);
			if (buffer)
			{
				return handle;
			}
			::CloseHandle(handle);
		}
		return nullptr;
	}
	void* OpenSharedMemoryRegion(void*& buffer, const wchar_t* name, size_t size, FlagSet<MemoryProtection> protection) noexcept
	{
		const auto memoryAccessWin32 = ConvertMemoryAccess(protection);
		void* handle = ::OpenFileMappingW(*memoryAccessWin32, FALSE, name);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			buffer = ::MapViewOfFile(handle, *memoryAccessWin32, 0, 0, size);
			if (buffer)
			{
				return handle;
			}
			::CloseHandle(handle);
		}
		return nullptr;
	}
	void FreeSharedMemoryRegion(void* handle, void* buffer) noexcept
	{
		if (buffer)
		{
			::UnmapViewOfFile(buffer);
		}
		if (handle)
		{
			::CloseHandle(handle);
		}
	}
}

namespace kxf
{
	void SharedMemoryBuffer::ZeroBuffer() noexcept
	{
		Utility::SecureZeroMemory(m_Buffer, m_Size);
	}

	std::unique_ptr<IInputStream> SharedMemoryBuffer::GetInputStream() const
	{
		if (!IsNull() && m_Protection.Contains(MemoryProtection::Read))
		{
			return std::make_unique<MemoryInputStream>(m_Buffer, m_Size);
		}
		return nullptr;
	}
	std::unique_ptr<IOutputStream> SharedMemoryBuffer::GetOutputStream()
	{
		if (!IsNull() && m_Protection.Contains(MemoryProtection::Write))
		{
			return std::make_unique<MemoryOutputStream>(m_Buffer, m_Size);
		}
		return nullptr;
	}
}
