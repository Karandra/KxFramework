#include "stdafx.h"
#include "SharedMemory.h"
#include <Windows.h>
#include "UndefWindows.h"

namespace
{
	constexpr DWORD ConvertProtection(KxFramework::MemoryProtection protection) noexcept
	{
		using namespace KxFramework;

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
		return 0;
	}
	constexpr DWORD ConvertMemoryAccess(KxFramework::MemoryProtection protection) noexcept
	{
		using namespace KxFramework;

		DWORD value = 0;
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

		if (protection & MemoryProtection::Execute)
		{
			value |= FILE_MAP_EXECUTE;
		}
		return value;
	}
}

namespace KxFramework::System
{
	void* AllocateSharedMemoryRegion(void*& buffer, size_t size, MemoryProtection protection, const wchar_t* name) noexcept
	{
		ULARGE_INTEGER sizeULI = {};
		sizeULI.QuadPart = size;

		const DWORD protectionWin32 = ConvertProtection(protection);
		const DWORD memoryAccessWin32 = ConvertMemoryAccess(protection);

		void* handle = ::CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, protectionWin32, sizeULI.HighPart, sizeULI.LowPart, name);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			buffer = ::MapViewOfFile(handle, memoryAccessWin32, 0, 0, size);
			if (buffer)
			{
				return handle;
			}
			::CloseHandle(handle);
		}
		return nullptr;
	}
	void* OpenSharedMemoryRegion(void*& buffer, const wchar_t* name, size_t size, MemoryProtection protection) noexcept
	{
		const DWORD memoryAccessWin32 = ConvertMemoryAccess(protection);
		void* handle = ::OpenFileMappingW(memoryAccessWin32, FALSE, name);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			buffer = ::MapViewOfFile(handle, memoryAccessWin32, 0, 0, size);
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

namespace KxFramework
{
	void SharedMemoryBuffer::ZeroBuffer() noexcept
	{
		::RtlSecureZeroMemory(m_Buffer, m_Size);
	}
}
