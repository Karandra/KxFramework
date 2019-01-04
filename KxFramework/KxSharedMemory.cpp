/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxSharedMemory.h"

namespace
{
	using Protection = KxSharedMemoryNS::Protection;

	DWORD ConvertProtection(uint32_t protection)
	{
		if (protection & Protection::Execute)
		{
			if (protection & Protection::RW)
			{
				return PAGE_EXECUTE_READWRITE;
			}
			if (protection & Protection::Read)
			{
				return PAGE_EXECUTE_READ;
			}
		}
		else
		{
			if (protection & Protection::RW)
			{
				return PAGE_READWRITE;
			}
			if (protection & Protection::Read)
			{
				return PAGE_READONLY;
			}
		}
		return 0;
	}
	DWORD ConvertMemoryAccess(uint32_t protection)
	{
		DWORD value = 0;
		if (protection & Protection::RW)
		{
			value = FILE_MAP_ALL_ACCESS;
		}
		else if (protection & Protection::Write)
		{
			value = FILE_MAP_WRITE;
		}
		else if (protection & Protection::Read)
		{
			value = FILE_MAP_READ;
		}

		if (protection & Protection::Execute)
		{
			value |= FILE_MAP_EXECUTE;
		}
		return value;
	}
}

namespace KxSharedMemoryNS
{
	bool Buffer::AllocateRegion(HANDLE& handle, void*& buffer, size_t size, uint32_t protection, const wchar_t* name)
	{
		ULARGE_INTEGER sizeULI = {0};
		sizeULI.QuadPart = size;

		const DWORD protectionWin32 = ConvertProtection(protection);
		const DWORD memoryAccessWin32 = ConvertMemoryAccess(protection);

		handle = ::CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, protectionWin32, sizeULI.HighPart, sizeULI.LowPart, name);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			buffer = ::MapViewOfFile(handle, memoryAccessWin32, 0, 0, size);
			if (buffer)
			{
				return true;
			}

			::CloseHandle(handle);
			handle = INVALID_HANDLE_VALUE;
		}
		return false;
	}
	bool Buffer::OpenRegion(HANDLE& handle, void*& buffer, const wchar_t* name, size_t size, uint32_t protection)
	{
		const DWORD memoryAccessWin32 = ConvertMemoryAccess(protection);
		handle = ::OpenFileMappingW(memoryAccessWin32, FALSE, name);
		if (handle != INVALID_HANDLE_VALUE)
		{
			buffer = ::MapViewOfFile(handle, memoryAccessWin32, 0, 0, size);
			if (buffer)
			{
				return true;
			}

			::CloseHandle(handle);
			handle = INVALID_HANDLE_VALUE;
		}
		return false;
	}
	void Buffer::FreeRegion(HANDLE handle, void* buffer)
	{
		::UnmapViewOfFile(buffer);
		::CloseHandle(handle);
	}
}
