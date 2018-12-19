/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxFileStream.h"
#include "KxFramework/KxUtility.h"
#include <wx/ustring.h>

namespace
{
	KxStreamBase::Offset GetFileSizeByHandle(HANDLE fileHandle)
	{
		LARGE_INTEGER size = {0};
		if (::GetFileSizeEx(fileHandle, &size))
		{
			return size.QuadPart;
		}
		return KxStreamBase::InvalidOffset;
	}
	KxStreamBase::Offset SeekByHandle(HANDLE fileHandle, KxStreamBase::Offset offset, KxStreamBase::SeekMode seekMode)
	{
		using SeekMode = KxStreamBase::SeekMode;

		DWORD seekModeWin = (DWORD)-1;
		switch (seekMode)
		{
			case SeekMode::FromStart:
			{
				seekModeWin = FILE_BEGIN;
				break;
			}
			case SeekMode::FromEnd:
			{
				seekModeWin = FILE_END;
				break;
			}
			case SeekMode::FromCurrent:
			{
				seekModeWin = FILE_CURRENT;
				break;
			}
			default:
			{
				return KxStreamBase::InvalidOffset;
			}
		};

		LARGE_INTEGER moveTo = {0};
		moveTo.QuadPart = offset;

		LARGE_INTEGER newOffset = {0};
		if (::SetFilePointerEx(fileHandle, moveTo, &newOffset, seekModeWin))
		{
			return newOffset.QuadPart;
		}
		return KxStreamBase::InvalidOffset;
	}
	KxStreamBase::Offset GetPositionByHandle(HANDLE fileHandle)
	{
		LARGE_INTEGER offset = {0};
		::SetFilePointerEx(fileHandle, offset, &offset, FILE_CURRENT);
		return offset.QuadPart;
	}
	wxString GetFileNameByHandle(HANDLE fileHandle)
	{
		wxString out;
		const DWORD flags = VOLUME_NAME_DOS|FILE_NAME_NORMALIZED;
		const DWORD length = ::GetFinalPathNameByHandleW(fileHandle, nullptr, 0, flags);
		if (length != 0)
		{
			::GetFinalPathNameByHandleW(fileHandle, wxStringBuffer(out, length), length, flags);
			out.erase(0, 4); // Remove '\\?\' prefix
		}
		return out;
	}

	KxStreamBase::ErrorCode TranslateErrorCode(DWORD winErrorCode, bool isWrite)
	{
		using ErrorCode = KxStreamBase::ErrorCode;

		if (winErrorCode == ERROR_SUCCESS)
		{
			return ErrorCode::Success;
		}
		else if (winErrorCode == ERROR_HANDLE_EOF)
		{
			return ErrorCode::EndOfFile;
		}
		else
		{
			return isWrite ? ErrorCode::WriteError : ErrorCode::ReadError;
		}
	}
	DWORD AccessModeToNative(int mode)
	{
		using Access = KxFileStream::Access;

		if (mode == Access::None)
		{
			return 0;
		}
		else if (mode != Access::Invalid)
		{
			DWORD nativeMode = 0;
			KxUtility::ModFlagRef(nativeMode, GENERIC_READ, mode & Access::Read);
			KxUtility::ModFlagRef(nativeMode, GENERIC_WRITE, mode & Access::Write);
			KxUtility::ModFlagRef(nativeMode, FILE_READ_ATTRIBUTES, mode & Access::ReadAttributes);
			KxUtility::ModFlagRef(nativeMode, FILE_WRITE_ATTRIBUTES, mode & Access::WriteAttributes);
			return nativeMode;
		}
		return Access::Invalid;
	}
	DWORD ShareModeToNative(int mode)
	{
		using Share = KxFileStream::Share;

		if (mode == Share::Exclusive)
		{
			return 0;
		}
		else if (mode != Share::Invalid)
		{
			DWORD nativeMode = 0;
			KxUtility::ModFlagRef(nativeMode, FILE_SHARE_READ, mode & Share::Read);
			KxUtility::ModFlagRef(nativeMode, FILE_SHARE_WRITE, mode & Share::Write);
			KxUtility::ModFlagRef(nativeMode, FILE_SHARE_DELETE, mode & Share::Delete);
			return nativeMode;
		}
		return Share::Invalid;
	}
	DWORD DispositionToNative(int mode)
	{
		using Disposition = KxFileStream::Disposition;

		switch (mode)
		{
			case Disposition::OpenExisting:
			{
				return OPEN_EXISTING;
			}
			case Disposition::OpenAlways:
			{
				return OPEN_ALWAYS;
			}
			case Disposition::CreateNew:
			{
				return CREATE_NEW;
			}
			case Disposition::CreateAlways:
			{
				return CREATE_ALWAYS;
			}
		};
		return Disposition::Invalid;
	}
	DWORD FlagsToNative(int flags)
	{
		using Flags = KxFileStream::Flags;

		DWORD nativeMode = 0;
		KxUtility::ModFlagRef(nativeMode, FILE_ATTRIBUTE_NORMAL, flags & Flags::Normal);
		KxUtility::ModFlagRef(nativeMode, FILE_FLAG_BACKUP_SEMANTICS, flags & Flags::BackupSemantics);
		return nativeMode;
	}
}

//////////////////////////////////////////////////////////////////////////
wxIMPLEMENT_CLASS2(KxFileStream, wxInputStream, wxOutputStream);

void KxFileStream::SetLastStreamError(DWORD error, bool isWrite)
{
	m_LastError = TranslateErrorCode(error, isWrite);
	wxInputStream::m_lasterror = (wxStreamError)m_LastError;
	wxOutputStream::m_lasterror = (wxStreamError)m_LastError;
}

wxFileOffset KxFileStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
	return SeekByHandle(m_Handle, pos, static_cast<SeekMode>(mode));
}
wxFileOffset KxFileStream::OnSysTell() const
{
	return GetPositionByHandle(m_Handle);
}
size_t KxFileStream::OnSysRead(void* buffer, size_t size)
{
	m_LastRead = 0;
	DWORD lastRead = 0;

	if (::ReadFile(m_Handle, buffer, size, &lastRead, nullptr))
	{
		SetLastStreamError(ERROR_SUCCESS, false);
	}
	else
	{
		SetLastStreamError(::GetLastError(), false);
	}

	m_Position = GetPositionByHandle(m_Handle);
	m_LastRead = lastRead;
	return m_LastRead;
}
size_t KxFileStream::OnSysWrite(const void* buffer, size_t size)
{
	m_LastWrite = 0;
	DWORD lastWrite = 0;

	if (::WriteFile(m_Handle, buffer, size, &lastWrite, nullptr))
	{
		SetLastStreamError(ERROR_SUCCESS, true);
	}
	else
	{
		SetLastStreamError(::GetLastError(), true);
	}

	m_Position = GetPositionByHandle(m_Handle);
	m_LastWrite = lastWrite;
	return m_LastWrite;
}

bool KxFileStream::IsOpened() const
{
	return m_Handle != INVALID_HANDLE_VALUE && m_Handle != nullptr;
}
bool KxFileStream::DoClose()
{
	if (IsOpened())
	{
		::CloseHandle(m_Handle);
		m_Handle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

KxFileStream::KxFileStream()
{
	SetLastStreamError(ERROR_SUCCESS, false);
}
KxFileStream::KxFileStream(HANDLE fileHandle, int accessMode, Disposition disposition, int shareMode, int flags)
{
	SetLastStreamError(ERROR_SUCCESS, false);
	Open(fileHandle, accessMode, disposition, shareMode, flags);
}
KxFileStream::KxFileStream(const wxString& filePath, int accessMode, Disposition disposition, int shareMode, int flags)
	:m_FilePath(filePath), m_AccessMode((Access)accessMode), m_Disposition(disposition), m_ShareMode((Share)shareMode), m_Flags((Flags)flags)
{
	SetLastStreamError(ERROR_SUCCESS, false);
	Open(filePath, accessMode, disposition, shareMode, flags);
}

bool KxFileStream::Open(HANDLE fileHandle, int accessMode, Disposition disposition, int shareMode, int flags)
{
	DoClose();

	m_Handle = fileHandle;
	m_AccessMode = (Access)accessMode;
	m_Disposition = disposition;
	m_ShareMode = (Share)shareMode;
	m_Flags = (Flags)flags;
	m_FilePath = GetFileName();

	return IsOk();
}
bool KxFileStream::Open(const wxString& filePath, int accessMode, Disposition disposition, int shareMode, int flags)
{
	DoClose();
	if (!filePath.IsEmpty())
	{
		m_AccessMode = (Access)accessMode;
		m_Disposition = disposition;
		m_ShareMode = (Share)shareMode;
		m_Flags = (Flags)flags;
		m_FilePath = filePath;
		m_Handle = ::CreateFileW(m_FilePath.wc_str(), AccessModeToNative(m_AccessMode), ShareModeToNative(m_ShareMode), nullptr, DispositionToNative(m_Disposition), FlagsToNative(flags), nullptr);

		return IsOk();
	}
	return false;
}

KxFileStream::~KxFileStream()
{
	DoClose();
}
bool KxFileStream::Close()
{
	return DoClose();
}

bool KxFileStream::IsOk() const
{
	return IsOpened();
}
bool KxFileStream::Eof() const
{
	return GetPositionByHandle(m_Handle) == GetFileSizeByHandle(m_Handle);
}
bool KxFileStream::CanRead() const
{
	return IsReadable() && !Eof();
}

size_t KxFileStream::GetSize() const
{
	return GetFileSizeByHandle(m_Handle);
}
KxFileStream::Offset KxFileStream::GetLength() const
{
	return GetFileSizeByHandle(m_Handle);
}

bool KxFileStream::IsSeekable() const
{
	return ::GetFileType(m_Handle) == FILE_TYPE_DISK;
}
KxFileStream::Offset KxFileStream::SeekI(wxFileOffset offset, wxSeekMode mode)
{
	return SeekByHandle(m_Handle, offset, static_cast<SeekMode>(mode));
}
KxFileStream::Offset KxFileStream::SeekO(wxFileOffset offset, wxSeekMode mode)
{
	return SeekByHandle(m_Handle, offset, static_cast<SeekMode>(mode));
}
KxFileStream::Offset KxFileStream::TellI() const
{
	return GetPositionByHandle(m_Handle);
}
KxFileStream::Offset KxFileStream::TellO() const
{
	return GetPositionByHandle(m_Handle);
}
KxFileStream::Offset KxFileStream::Tell() const
{
	return GetPositionByHandle(m_Handle);
}
KxFileStream::Offset KxFileStream::Seek(Offset offset, SeekMode mode)
{
	return SeekByHandle(m_Handle, offset, mode);
}

char KxFileStream::Peek()
{
	if (!Eof())
	{
		char c = ReadObject<char>();
		Skip(-1);
		return c;
	}
	return 0;
}

size_t KxFileStream::LastRead() const
{
	return m_LastRead;
}
size_t KxFileStream::LastWrite() const
{
	return m_LastWrite;
}
wxStreamError KxFileStream::GetLastError() const
{
	return static_cast<wxStreamError>(m_LastError);
}

bool KxFileStream::IsWriteable() const
{
	return m_AccessMode & Access::Write || m_AccessMode & Access::WriteAttributes;
}
bool KxFileStream::IsReadable() const
{
	return m_AccessMode & Access::Read || m_AccessMode & Access::ReadAttributes;
}

bool KxFileStream::Flush()
{
	return ::FlushFileBuffers(m_Handle);
}
bool KxFileStream::SetAllocationSize(Offset offset)
{
	if (offset != InvalidOffset)
	{
		Offset oldPos = GetPositionByHandle(m_Handle);
		SeekByHandle(m_Handle, offset, SeekMode::FromStart);

		bool success = ::SetEndOfFile(m_Handle);
		SeekByHandle(m_Handle, oldPos, SeekMode::FromStart);
		return success;
	}
	else
	{
		return ::SetEndOfFile(m_Handle);
	}
}

wxString KxFileStream::GetFileName() const
{
	if (m_FilePath.IsEmpty())
	{
		m_FilePath = GetFileNameByHandle(m_Handle);
	}
	return m_FilePath;
}
HANDLE KxFileStream::GetHandle() const
{
	return m_Handle;
}

KxFileStream::operator wxStreamBase*()
{
	if (IsWriteable())
	{
		return static_cast<wxOutputStream*>(this);
	}
	else
	{
		return static_cast<wxInputStream*>(this);
	}
}
KxFileStream::operator const wxStreamBase*()
{
	if (IsWriteable())
	{
		return static_cast<wxOutputStream*>(this);
	}
	else
	{
		return static_cast<wxInputStream*>(this);
	}
}
