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
	bool Buffer::Open(const wxString& name, size_t size, uint32_t protection)
	{
		if (!name.IsEmpty())
		{
			const DWORD memoryAccessWin32 = ConvertMemoryAccess(protection);
			m_Handle = ::OpenFileMappingW(memoryAccessWin32, FALSE, name);
			if (m_Handle != INVALID_HANDLE_VALUE)
			{
				m_Buffer = ::MapViewOfFile(m_Handle, memoryAccessWin32, 0, 0, size);
				m_Size = size;
				m_Protection = static_cast<Protection>(protection);

				if (m_Buffer)
				{
					return true;
				}
				::CloseHandle(m_Handle);
			}
		}
		return false;
	}
	bool Buffer::Allocate(size_t size, uint32_t protection, const wxString& name)
	{
		ULARGE_INTEGER sizeULI = {0};
		sizeULI.QuadPart = size;

		const DWORD protectionWin32 = ConvertProtection(protection);
		const DWORD memoryAccessWin32 = ConvertMemoryAccess(protection);

		m_Handle = ::CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, protectionWin32, sizeULI.HighPart, sizeULI.LowPart, name.IsEmpty() ? NULL : name.wc_str());
		if (m_Handle != INVALID_HANDLE_VALUE)
		{
			m_Buffer = ::MapViewOfFile(m_Handle, memoryAccessWin32, 0, 0, size);
			m_Size = size;
			m_Protection = static_cast<Protection>(protection);

			if (m_Buffer)
			{
				return true;
			}
			::CloseHandle(m_Handle);
		}
		return false;
	}
	void Buffer::Free()
	{
		::UnmapViewOfFile(m_Buffer);
		::CloseHandle(m_Handle);
	}

	bool Buffer::CreateFrom(const Buffer& other)
	{
		FreeIfNeeded();
		if (other.IsOK())
		{
			// Allocate new unnamed buffer with same parameters
			return Allocate(other.m_Size, other.m_Protection);
		}
		return false;
	}
}
