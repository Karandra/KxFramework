#include "stdafx.h"
#include "FileStream.h"
#include "kxf/System/ErrorCodeValue.h"
#include "kxf/FileSystem/Private/NativeFSUtility.h"

namespace
{
	using namespace kxf;

	wxFileOffset GetFileSizeByHandle(HANDLE handle) noexcept
	{
		LARGE_INTEGER size = {};
		if (::GetFileSizeEx(handle, &size))
		{
			return size.QuadPart;
		}
		return wxInvalidOffset;
	}
	wxFileOffset SeekByHandle(HANDLE handle, BinarySize offset, StreamSeekMode seekMode) noexcept
	{
		DWORD seekModeWin = std::numeric_limits<DWORD>::max();
		switch (seekMode)
		{
			case StreamSeekMode::FromStart:
			{
				seekModeWin = FILE_BEGIN;
				break;
			}
			case StreamSeekMode::FromCurrent:
			{
				seekModeWin = FILE_CURRENT;
				break;
			}
			case StreamSeekMode::FromEnd:
			{
				seekModeWin = FILE_END;
				break;
			}
			default:
			{
				return wxInvalidOffset;
			}
		};

		LARGE_INTEGER moveTo = {};
		moveTo.QuadPart = offset.GetBytes();

		LARGE_INTEGER newOffset = {};
		if (::SetFilePointerEx(handle, moveTo, &newOffset, seekModeWin))
		{
			return newOffset.QuadPart;
		}
		return wxInvalidOffset;
	}
	wxFileOffset GetPositionByHandle(HANDLE handle) noexcept
	{
		LARGE_INTEGER offset = {};
		::SetFilePointerEx(handle, offset, &offset, FILE_CURRENT);
		return offset.QuadPart;
	}
	FSPath GetFileNameByHandle(HANDLE handle)
	{
		constexpr DWORD flags = VOLUME_NAME_DOS|FILE_NAME_NORMALIZED;
		const DWORD length = ::GetFinalPathNameByHandleW(handle, nullptr, 0, flags);
		if (length != 0)
		{
			String result;
			if (::GetFinalPathNameByHandleW(handle, wxStringBuffer(result, length), length, flags) != 0)
			{
				return FSPath(std::move(result));
			}
		}
		return {};
	}

	constexpr StreamErrorCode TranslateErrorCode(Win32Error win32Error, bool isWrite) noexcept
	{
		if (win32Error.GetValue() == ERROR_SUCCESS)
		{
			return StreamErrorCode::Success;
		}
		else if (win32Error.GetValue() == ERROR_HANDLE_EOF)
		{
			return StreamErrorCode::EndOfStream;
		}
		else
		{
			return isWrite ? StreamErrorCode::WriteError : StreamErrorCode::ReadError;
		}
	}
}

namespace kxf
{
	wxIMPLEMENT_CLASS2(FileStream, wxInputStream, wxOutputStream);

	void FileStream::DoSetLastError(uint32_t error, bool isWrite)
	{
		m_LastError = TranslateErrorCode(error, isWrite);
		wxInputStream::m_lasterror = static_cast<wxStreamError>(m_LastError);
		wxOutputStream::m_lasterror = static_cast<wxStreamError>(m_LastError);
	}

	wxFileOffset FileStream::OnSysTell() const
	{
		return GetPositionByHandle(m_Handle);
	}
	wxFileOffset FileStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
	{
		return SeekByHandle(m_Handle, pos, static_cast<StreamSeekMode>(mode));
	}
	size_t FileStream::OnSysRead(void* buffer, size_t size)
	{
		m_LastRead = 0;
		DWORD lastRead = 0;

		if (::ReadFile(m_Handle, buffer, size, &lastRead, nullptr))
		{
			DoSetLastError(ERROR_SUCCESS, false);
		}
		else
		{
			DoSetLastError(::GetLastError(), false);
		}

		m_Position = GetPositionByHandle(m_Handle);
		m_LastRead = lastRead;
		return m_LastRead;
	}
	size_t FileStream::OnSysWrite(const void* buffer, size_t size)
	{
		m_LastWrite = 0;
		DWORD lastWrite = 0;

		if (::WriteFile(m_Handle, buffer, size, &lastWrite, nullptr))
		{
			DoSetLastError(ERROR_SUCCESS, true);
		}
		else
		{
			DoSetLastError(::GetLastError(), true);
		}

		m_Position = GetPositionByHandle(m_Handle);
		m_LastWrite = lastWrite;
		return m_LastWrite;
	}

	bool FileStream::DoIsOpened() const
	{
		return m_Handle && m_Handle != INVALID_HANDLE_VALUE;
	}
	bool FileStream::DoClose()
	{
		if (DoIsOpened())
		{
			Flush();
			::CloseHandle(m_Handle);

			m_Handle = INVALID_HANDLE_VALUE;
			return true;
		}
		return false;
	}

	FSPath FileStream::GetFileSystemPath() const
	{
		return GetFileNameByHandle(m_Handle);
	}
	bool FileStream::AttachHandle(void* handle, FlagSet<FileStreamAccess> access, FileStreamDisposition disposition, FlagSet<FileStreamShare> share, FlagSet<FileStreamFlags> flags)
	{
		DoClose();
		DoSetLastError(0, false);

		m_AccessMode = access;
		m_Disposition = disposition;
		m_ShareMode = share;
		m_Flags = flags;
		m_Handle = handle;
		return DoIsOpened();
	}
	bool FileStream::ReopenHandle(void* handle, FlagSet<FileStreamAccess> access, FileStreamDisposition disposition, FlagSet<FileStreamShare> share, FlagSet<FileStreamFlags> flags)
	{
		DoClose();
		DoSetLastError(0, false);

		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			m_AccessMode = access;
			m_Disposition = disposition;
			m_ShareMode = share;
			m_Flags = flags;
			m_Handle = ::ReOpenFile(handle,
									FileSystem::Private::MapFileAccessMode(m_AccessMode),
									FileSystem::Private::MapFileShareMode(m_ShareMode),
									FileSystem::Private::MapFileFlags(flags));

			return DoIsOpened();
		}
		return false;
	}
	void* FileStream::DetachHandle()
	{
		void* handle = m_Handle;
		m_Handle = nullptr;
		return handle;
	}
	bool FileStream::Open(const FSPath& path, FlagSet<FileStreamAccess> access, FileStreamDisposition disposition, FlagSet<FileStreamShare> share, FlagSet<FileStreamFlags> flags)
	{
		DoClose();
		DoSetLastError(0, false);

		if (path)
		{
			m_AccessMode = access;
			m_Disposition = disposition;
			m_ShareMode = share;
			m_Flags = flags;

			String pathString = path.GetFullPathWithNS(FSPathNamespace::Win32File);
			m_Handle = ::CreateFileW(pathString.wc_str(),
									 FileSystem::Private::MapFileAccessMode(m_AccessMode),
									 FileSystem::Private::MapFileShareMode(m_ShareMode),
									 nullptr,
									 FileSystem::Private::MapFileDisposition(m_Disposition),
									 FileSystem::Private::MapFileFlags(flags),
									 nullptr);
			return DoIsOpened();
		}
		return false;
	}

	bool FileStream::Eof() const
	{
		return GetPositionByHandle(m_Handle) == GetFileSizeByHandle(m_Handle);
	}
	bool FileStream::CanRead() const
	{
		return IsReadable() && !Eof();
	}
	size_t FileStream::GetSize() const
	{
		return GetFileSizeByHandle(m_Handle);
	}
	wxFileOffset FileStream::GetLength() const
	{
		return GetFileSizeByHandle(m_Handle);
	}

	bool FileStream::IsSeekable() const
	{
		return ::GetFileType(m_Handle) == FILE_TYPE_DISK;
	}
	wxFileOffset FileStream::SeekI(wxFileOffset offset, wxSeekMode mode)
	{
		return SeekByHandle(m_Handle, offset, static_cast<StreamSeekMode>(mode));
	}
	wxFileOffset FileStream::SeekO(wxFileOffset offset, wxSeekMode mode)
	{
		return SeekByHandle(m_Handle, offset, static_cast<StreamSeekMode>(mode));
	}
	wxFileOffset FileStream::TellI() const
	{
		return GetPositionByHandle(m_Handle);
	}
	wxFileOffset FileStream::TellO() const
	{
		return GetPositionByHandle(m_Handle);
	}

	bool FileStream::Flush()
	{
		return ::FlushFileBuffers(m_Handle);
	}
	bool FileStream::SetAllocationSize(BinarySize offset)
	{
		if (offset)
		{
			wxFileOffset oldPos = GetPositionByHandle(m_Handle);
			SeekByHandle(m_Handle, offset, StreamSeekMode::FromStart);

			bool success = ::SetEndOfFile(m_Handle);
			SeekByHandle(m_Handle, oldPos, StreamSeekMode::FromStart);
			return success;
		}
		return ::SetEndOfFile(m_Handle);
	}

	FlagSet<FileAttribute> FileStream::GetAttributes() const
	{
		if (m_Handle)
		{
			BY_HANDLE_FILE_INFORMATION info = {};
			if (::GetFileInformationByHandle(m_Handle, &info))
			{
				return FileSystem::Private::MapFileAttributes(info.dwFileAttributes);
			}
		}
		return FileAttribute::Invalid;
	}
	bool FileStream::SetAttributes(FlagSet<FileAttribute> attributes)
	{
		if (m_Handle)
		{
			FILE_BASIC_INFO basicInfo = {};
			if (::GetFileInformationByHandleEx(m_Handle, FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo, &basicInfo, sizeof(basicInfo)))
			{
				basicInfo.FileAttributes = FileSystem::Private::MapFileAttributes(attributes);
				return ::SetFileInformationByHandle(m_Handle, FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo, &basicInfo, sizeof(basicInfo));
			}
		}
		return false;
	}

	bool FileStream::GetTimestamp(DateTime& creationTime, DateTime& modificationTime, DateTime& lastAccessTime) const
	{
		FILETIME creationTimeFile = {};
		FILETIME modificationTimeFile = {};
		FILETIME lastAccessTimeFile = {};
		if (m_Handle && ::GetFileTime(m_Handle, &creationTimeFile, &lastAccessTimeFile, &modificationTimeFile))
		{
			creationTime = FileSystem::Private::ConvertDateTime(creationTimeFile);
			modificationTime = FileSystem::Private::ConvertDateTime(modificationTimeFile);
			lastAccessTime = FileSystem::Private::ConvertDateTime(lastAccessTimeFile);

			return true;
		}
		return false;
	}
	bool FileStream::ChangeTimestamp(DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime)
	{
		if (m_Handle)
		{
			auto ToFileTime = [](DateTime dateTime)
			{
				return FileSystem::Private::ConvertDateTimeToFileTime(dateTime).value_or(FILETIME{0, 0});
			};

			FILETIME creationTimeFile = ToFileTime(creationTime);
			FILETIME modificationTimeFile = ToFileTime(modificationTime);
			FILETIME lastAccessTimeFile = ToFileTime(lastAccessTime);
			return ::SetFileTime(m_Handle, &creationTimeFile, &lastAccessTimeFile, &modificationTimeFile);
		}
		return false;
	}
}
