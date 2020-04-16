#include "KxStdAfx.h"
#include "FileStream.h"
#include "NativeFileSystemUtility.h"

namespace
{
	wxFileOffset GetFileSizeByHandle(HANDLE fileHandle)
	{
		LARGE_INTEGER size = {};
		if (::GetFileSizeEx(fileHandle, &size))
		{
			return size.QuadPart;
		}
		return wxInvalidOffset;
	}
	wxFileOffset SeekByHandle(HANDLE fileHandle, KxFramework::BinarySize offset, KxFramework::StreamSeekMode seekMode)
	{
		using namespace KxFramework;

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
		if (::SetFilePointerEx(fileHandle, moveTo, &newOffset, seekModeWin))
		{
			return newOffset.QuadPart;
		}
		return wxInvalidOffset;
	}
	wxFileOffset GetPositionByHandle(HANDLE fileHandle)
	{
		LARGE_INTEGER offset = {0};
		::SetFilePointerEx(fileHandle, offset, &offset, FILE_CURRENT);
		return offset.QuadPart;
	}
	KxFramework::FSPath GetFileNameByHandle(HANDLE fileHandle)
	{
		using namespace KxFramework;

		constexpr DWORD flags = VOLUME_NAME_DOS|FILE_NAME_NORMALIZED;
		const DWORD length = ::GetFinalPathNameByHandleW(fileHandle, nullptr, 0, flags);
		if (length != 0)
		{
			String result;
			if (::GetFinalPathNameByHandleW(fileHandle, wxStringBuffer(result, length), length, flags) != 0)
			{
				return FSPath(std::move(result));
			}
		}
		return {};
	}

	KxFramework::StreamErrorCode TranslateErrorCode(DWORD winErrorCode, bool isWrite)
	{
		using namespace KxFramework;

		if (winErrorCode == ERROR_SUCCESS)
		{
			return StreamErrorCode::Success;
		}
		else if (winErrorCode == ERROR_HANDLE_EOF)
		{
			return StreamErrorCode::EndOfstream;
		}
		else
		{
			return isWrite ? StreamErrorCode::WriteError : StreamErrorCode::ReadError;
		}
	}
	DWORD AccessModeToNative(KxFramework::FileStreamAccess mode)
	{
		using namespace KxFramework;

		if (mode == FileStreamAccess::None)
		{
			return 0;
		}
		else
		{
			DWORD nativeMode = 0;
			Utility::AddFlagRef(nativeMode, GENERIC_READ, mode & FileStreamAccess::Read);
			Utility::AddFlagRef(nativeMode, GENERIC_WRITE, mode & FileStreamAccess::Write);
			Utility::AddFlagRef(nativeMode, FILE_READ_ATTRIBUTES, mode & FileStreamAccess::ReadAttributes);
			Utility::AddFlagRef(nativeMode, FILE_WRITE_ATTRIBUTES, mode & FileStreamAccess::WriteAttributes);
			return nativeMode;
		}
		return std::numeric_limits<DWORD>::max();
	}
	DWORD ShareModeToNative(KxFramework::FileStreamShare mode)
	{
		using namespace KxFramework;

		if (mode == FileStreamShare::None)
		{
			return 0;
		}
		else
		{
			DWORD nativeMode = 0;
			Utility::AddFlagRef(nativeMode, FILE_SHARE_READ, mode & FileStreamShare::Read);
			Utility::AddFlagRef(nativeMode, FILE_SHARE_WRITE, mode & FileStreamShare::Write);
			Utility::AddFlagRef(nativeMode, FILE_SHARE_DELETE, mode & FileStreamShare::Delete);
			return nativeMode;
		}
		return std::numeric_limits<DWORD>::max();
	}
	DWORD DispositionToNative(KxFramework::FileStreamDisposition mode)
	{
		using namespace KxFramework;

		switch (mode)
		{
			case FileStreamDisposition::OpenExisting:
			{
				return OPEN_EXISTING;
			}
			case FileStreamDisposition::OpenAlways:
			{
				return OPEN_ALWAYS;
			}
			case FileStreamDisposition::CreateNew:
			{
				return CREATE_NEW;
			}
			case FileStreamDisposition::CreateAlways:
			{
				return CREATE_ALWAYS;
			}
		};
		return 0;
	}
	DWORD FlagsToNative(KxFramework::FileStreamFlags flags)
	{
		using namespace KxFramework;

		DWORD nativeMode = 0;
		Utility::AddFlagRef(nativeMode, FILE_ATTRIBUTE_NORMAL, flags & FileStreamFlags::Normal);
		Utility::AddFlagRef(nativeMode, FILE_FLAG_BACKUP_SEMANTICS, flags & FileStreamFlags::BackupSemantics);
		return nativeMode;
	}
}

namespace KxFramework
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
	bool FileStream::AttachHandle(void* handle)
	{
		DoClose();
		DoSetLastError(0, false);

		m_AccessMode = FileStreamAccess::None;
		m_Disposition = FileStreamDisposition::OpenExisting;
		m_ShareMode = FileStreamShare::None;
		m_Flags = FileStreamFlags::None;
		m_Handle = handle;
		return DoIsOpened();
	}
	bool FileStream::AttachHandle(void* handle, FileStreamAccess access, FileStreamDisposition disposition, FileStreamShare share, FileStreamFlags flags)
	{
		DoClose();
		DoSetLastError(0, false);

		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			m_Handle = handle;
			m_AccessMode = access;
			m_Disposition = disposition;
			m_ShareMode = share;
			m_Flags = flags;

			m_Handle = ::ReOpenFile(handle, AccessModeToNative(m_AccessMode), ShareModeToNative(m_ShareMode), FlagsToNative(flags));
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
	bool FileStream::Open(const FSPath& path, FileStreamAccess access, FileStreamDisposition disposition, FileStreamShare share, FileStreamFlags flags)
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
			m_Handle = ::CreateFileW(pathString.wc_str(), AccessModeToNative(m_AccessMode), ShareModeToNative(m_ShareMode), nullptr, DispositionToNative(m_Disposition), FlagsToNative(flags), nullptr);
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

	FileAttribute FileStream::GetAttributes() const
	{
		if (m_Handle)
		{
			BY_HANDLE_FILE_INFORMATION info = {};
			if (::GetFileInformationByHandle(m_Handle, &info))
			{
				return FileSystem::NativeUtility::MapFileAttributes(info.dwFileAttributes);
			}
		}
		return FileAttribute::Invalid;
	}
	bool FileStream::SetAttributes(FileAttribute attributes)
	{
		if (m_Handle)
		{
			FILE_BASIC_INFO basicInfo = {};
			if (::GetFileInformationByHandleEx(m_Handle, FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo, &basicInfo, sizeof(basicInfo)))
			{
				basicInfo.FileAttributes = FileSystem::NativeUtility::MapFileAttributes(attributes);
				return ::SetFileInformationByHandle(m_Handle, FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo, &basicInfo, sizeof(basicInfo));
			}
		}
		return false;
	}

	bool FileStream::GetTimestamp(wxDateTime& creationTime, wxDateTime& modificationTime, wxDateTime& lastAccessTime) const
	{
		FILETIME creationTimeFile = {};
		FILETIME modificationTimeFile = {};
		FILETIME lastAccessTimeFile = {};
		if (m_Handle && ::GetFileTime(m_Handle, &creationTimeFile, &lastAccessTimeFile, &modificationTimeFile))
		{
			auto ToDateTime = [](const FILETIME& fileTime)
			{
				SYSTEMTIME systemTime = {};
				SYSTEMTIME localTime = {};
				if (::FileTimeToSystemTime(&fileTime, &systemTime) && ::SystemTimeToTzSpecificLocalTime(nullptr, &systemTime, &localTime))
				{
					return wxDateTime().SetFromMSWSysTime(localTime);
				}
				return wxInvalidDateTime;
			};

			creationTime = ToDateTime(creationTimeFile);
			modificationTime = ToDateTime(modificationTimeFile);
			lastAccessTime = ToDateTime(lastAccessTimeFile);

			return true;
		}
		return false;
	}
	bool FileStream::ChangeTimestamp(const wxDateTime& creationTime, const wxDateTime& modificationTime, const wxDateTime& lastAccessTime)
	{
		if (m_Handle)
		{
			auto ToFileTime = [](const wxDateTime& dateTime)
			{
				FILETIME fileTime = {0, 0};
				if (dateTime.IsValid())
				{
					SYSTEMTIME localTime = {};
					SYSTEMTIME systemTime = {};
					dateTime.GetAsMSWSysTime(&localTime);

					if (::TzSpecificLocalTimeToSystemTime(nullptr, &localTime, &systemTime) && ::SystemTimeToFileTime(&systemTime, &fileTime))
					{
						return fileTime;
					}
				}
				return fileTime;
			};

			FILETIME creationTimeFile = ToFileTime(creationTime);
			FILETIME modificationTimeFile = ToFileTime(modificationTime);
			FILETIME lastAccessTimeFile = ToFileTime(lastAccessTime);
			return ::SetFileTime(m_Handle, &creationTimeFile, &modificationTimeFile, &lastAccessTimeFile);
		}
		return false;
	}
}
