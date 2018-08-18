#include "KxStdAfx.h"
#include "KxFramework/KxFileStream.h"
#include <wx/ustring.h>

wxIMPLEMENT_CLASS2(KxFileStream, wxInputStream, wxOutputStream);

wxString KxFileStream::GetFileNameByHandle(HANDLE fileHandle)
{
	wxString out;
	const DWORD flags = VOLUME_NAME_DOS|FILE_NAME_NORMALIZED;
	DWORD length = ::GetFinalPathNameByHandleW(fileHandle, NULL, 0, flags);
	if (length != 0)
	{
		::GetFinalPathNameByHandleW(fileHandle, wxStringBuffer(out, length), length, flags);
		out.erase(0, 4); // Remove "\\?\" prefix
	}
	return out;
}
wxFileOffset KxFileStream::GetFileSizeByHandle(HANDLE fileHandle)
{
	LARGE_INTEGER size = {0};
	if (::GetFileSizeEx(fileHandle, &size))
	{
		return size.QuadPart;
	}
	return wxInvalidOffset;
}

wxStreamError KxFileStream::TranslateErrorCode(DWORD error, bool isWrite)
{
	if (error == ERROR_SUCCESS)
	{
		return wxSTREAM_NO_ERROR;
	}
	else if (error == ERROR_HANDLE_EOF)
	{
		return wxSTREAM_EOF;
	}
	else
	{
		return isWrite ? wxSTREAM_WRITE_ERROR : wxSTREAM_READ_ERROR;
	}
}
DWORD KxFileStream::SeekModeToNative(KxFileStreamSeek mode)
{
	switch (mode)
	{
		case KxFS_SEEK_CURRENT:
		{
			return FILE_CURRENT;
		}
		case KxFS_SEEK_BEGIN:
		{
			return FILE_BEGIN;
		}
		case KxFS_SEEK_END:
		{
			return FILE_END;
		}
	};
	return (DWORD)KxFS_SEEK_INVALID;
}
DWORD KxFileStream::AccessModeToNative(int mode)
{
	if (mode == KxFS_ACCESS_NONE)
	{
		return 0;
	}
	else if (mode != KxFS_ACCESS_INVALID)
	{
		DWORD nativeMode = 0;
		KxUtility::ModFlagRef(nativeMode, GENERIC_READ, mode & KxFS_ACCESS_READ);
		KxUtility::ModFlagRef(nativeMode, GENERIC_WRITE, mode & KxFS_ACCESS_WRITE);
		KxUtility::ModFlagRef(nativeMode, FILE_READ_ATTRIBUTES, mode & KxFS_ACCESS_READ_ATTRIBUTES);
		KxUtility::ModFlagRef(nativeMode, FILE_WRITE_ATTRIBUTES, mode & KxFS_ACCESS_WRITE_ATTRIBUTES);
		return nativeMode;
	}
	return (DWORD)KxFS_ACCESS_INVALID;
}
DWORD KxFileStream::ShareModeToNative(int mode)
{
	if (mode == KxFS_SHARE_EXCLUSIVE)
	{
		return 0;
	}
	else if (mode != KxFS_SHARE_INVALID)
	{
		DWORD nativeMode = 0;
		KxUtility::ModFlagRef(nativeMode, FILE_SHARE_READ, mode & KxFS_SHARE_READ);
		KxUtility::ModFlagRef(nativeMode, FILE_SHARE_WRITE, mode & KxFS_SHARE_WRITE);
		KxUtility::ModFlagRef(nativeMode, FILE_SHARE_DELETE, mode & KxFS_SHARE_DELETE);
		return nativeMode;
	}
	return (DWORD)KxFS_SHARE_INVALID;
}
DWORD KxFileStream::DispositionToNative(KxFileStreamDisposition mode)
{
	switch (mode)
	{
		case KxFS_DISP_OPEN_EXISTING:
		{
			return OPEN_EXISTING;
		}
		case KxFS_DISP_OPEN_ALWAYS:
		{
			return OPEN_ALWAYS;
		}
		case KxFS_DISP_CREATE_NEW:
		{
			return CREATE_NEW;
		}
		case KxFS_DISP_CREATE_ALWAYS:
		{
			return CREATE_ALWAYS;
		}
	};
	return (DWORD)KxFS_DISP_INVALID;
}
DWORD KxFileStream::FlagsToNative(int flags)
{
	DWORD nativeFlags = 0;
	if (flags & KxFS_FLAG_NORMAL)
	{
		nativeFlags |= FILE_ATTRIBUTE_NORMAL;
	}
	if (flags & KxFS_FLAG_BACKUP_SEMANTICS)
	{
		nativeFlags |= FILE_FLAG_BACKUP_SEMANTICS;
	}
	return nativeFlags;
}
KxFileStreamSeek KxFileStream::WxSeekModeToKx(wxSeekMode mode)
{
	switch (mode)
	{
		case wxFromCurrent:
		{
			return KxFS_SEEK_CURRENT;
		}
		case wxFromStart:
		{
			return KxFS_SEEK_BEGIN;
		}
		case wxFromEnd:
		{
			return KxFS_SEEK_END;
		}
	};
	return KxFS_SEEK_INVALID;
}

void KxFileStream::SetError(DWORD error, bool isWrite)
{
	m_LastError = TranslateErrorCode(error, isWrite);
	wxInputStream::m_lasterror = m_LastError;
	wxOutputStream::m_lasterror = m_LastError;
}

wxFileOffset KxFileStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
	return Seek(pos, WxSeekModeToKx(mode));
}
wxFileOffset KxFileStream::OnSysTell() const
{
	return GetPosition();
}
size_t KxFileStream::OnSysRead(void* buffer, size_t size)
{
	m_LastRead = 0;
	if (::ReadFile(m_Handle, buffer, size, &m_LastRead, NULL))
	{
		m_Position = GetPosition();
		SetError(ERROR_SUCCESS, false);
		return m_LastRead;
	}
	else
	{
		SetError(::GetLastError(), false);
		return 0;
	}
}
size_t KxFileStream::OnSysWrite(const void* buffer, size_t size)
{
	m_LastWrite = 0;
	if (::WriteFile(m_Handle, buffer, size, &m_LastWrite, NULL))
	{
		m_Position = GetPosition();
		SetError(ERROR_SUCCESS, true);
		return m_LastWrite;
	}
	else
	{
		SetError(::GetLastError(), true);
		return 0;
	}
}

KxFileStream::KxFileStream()
{
}
KxFileStream::KxFileStream(HANDLE fileHandle, int accessMode, KxFileStreamDisposition disposition, int shareMode, int flags)
{
	Open(fileHandle, accessMode, disposition, shareMode, flags);
}
KxFileStream::KxFileStream(const wxString& filePath, int accessMode, KxFileStreamDisposition disposition, int shareMode, int flags)
	:m_FilePath(filePath), m_AccessMode((KxFileStreamAccess)accessMode), m_Disposition(disposition), m_ShareMode((KxFileStreamShare)shareMode), m_Flags((KxFileStreamFlags)flags)
{
	Open(filePath, accessMode, disposition, shareMode, flags);
}

bool KxFileStream::Open(HANDLE fileHandle, int accessMode, KxFileStreamDisposition disposition, int shareMode, int flags)
{
	if (!IsOpened())
	{
		m_Handle = fileHandle;
		m_AccessMode = (KxFileStreamAccess)accessMode;
		m_Disposition = disposition;
		m_ShareMode = (KxFileStreamShare)shareMode;
		m_Flags = (KxFileStreamFlags)flags;
		m_FilePath = GetFileName();

		return IsOk();
	}
	return false;
}
bool KxFileStream::Open(const wxString& filePath, int accessMode, KxFileStreamDisposition disposition, int shareMode, int flags)
{
	if (!IsOpened() && !filePath.IsEmpty())
	{
		m_AccessMode = (KxFileStreamAccess)accessMode;
		m_Disposition = disposition;
		m_ShareMode = (KxFileStreamShare)shareMode;
		m_Flags = (KxFileStreamFlags)flags;
		m_FilePath = filePath;
		m_Handle = ::CreateFileW(m_FilePath, AccessModeToNative(m_AccessMode), ShareModeToNative(m_ShareMode), NULL, DispositionToNative(m_Disposition), FlagsToNative(flags), NULL);

		return IsOk();
	}
	return false;
}

KxFileStream::~KxFileStream()
{
	Close();
}
bool KxFileStream::Close()
{
	if (IsOpened() && m_Handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_Handle);
		m_Handle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

bool KxFileStream::IsOk() const
{
	return IsOpened() && m_Handle != INVALID_HANDLE_VALUE && m_Handle != NULL;
}
wxFileOffset KxFileStream::Seek(wxFileOffset offset, KxFileStreamSeek mode)
{
	DWORD winMode = SeekModeToNative(mode);
	if (winMode != (DWORD)KxFS_SEEK_INVALID)
	{
		LARGE_INTEGER offsetLI = {0};
		offsetLI.QuadPart = offset;

		LARGE_INTEGER newOffset = {0};
		if (SetFilePointerEx(m_Handle, offsetLI, &newOffset, winMode))
		{
			m_Position = newOffset.QuadPart;
			return m_Position;
		}
	}
	return wxInvalidOffset;
}

wxString KxFileStream::ReadStringCurrentLocale(size_t size, bool* isSuccess)
{
	bool isOK = false;
	auto buffer = ReadData<std::vector<char>>(size, &isOK);
	KxUtility::SetIfNotNull(isSuccess, isOK);

	if (isOK)
	{
		return wxString(buffer.data(), buffer.size());
	}
	return wxEmptyString;
}
bool KxFileStream::WriteStringCurrentLocale(const wxString& v)
{
	auto data = v.c_str().AsCharBuf();
	return OnSysWrite(data.data(), data.length()) != 0;
}
wxString KxFileStream::ReadStringASCII(size_t size, bool* isSuccess)
{
	bool isOK = false;
	auto buffer = ReadData<std::vector<char>>(size, &isOK);
	KxUtility::SetIfNotNull(isSuccess, isOK);

	if (isOK)
	{
		return wxString::FromAscii(buffer.data(), buffer.size());
	}
	return wxEmptyString;
}
bool KxFileStream::WriteStringASCII(const wxString& v, char replacement)
{
	auto data = v.ToAscii(replacement);
	return OnSysWrite(data.data(), data.length()) != 0;
}
wxString KxFileStream::ReadStringUTF8(size_t size, bool* isSuccess)
{
	bool isOK = false;
	auto buffer = ReadData<std::vector<char>>(size, &isOK);
	KxUtility::SetIfNotNull(isSuccess, isOK);

	if (isOK)
	{
		return wxString::FromUTF8(buffer.data(), buffer.size());
	}
	return wxEmptyString;
}
bool KxFileStream::WriteStringUTF8(const wxString& v)
{
	auto utf8 = v.ToUTF8();
	return OnSysWrite(utf8.data(), utf8.length()) != 0;
}
wxString KxFileStream::ReadStringUTF16(size_t size, bool* isSuccess)
{
	bool isOK = false;
	auto buffer = ReadData<std::vector<wchar_t>>(size, &isOK);
	KxUtility::SetIfNotNull(isSuccess, isOK);

	if (isOK)
	{
		return wxString(buffer.data(), buffer.size());
	}
	return wxEmptyString;
}
bool KxFileStream::WriteStringUTF16(const wxString& v)
{
	auto utf8 = v.wchar_str();
	return OnSysWrite(utf8.data(), utf8.length()) != 0;
}
wxString KxFileStream::ReadStringUTF32(size_t size, bool* isSuccess)
{
	bool isOK = false;
	auto buffer = ReadData<std::vector<wxChar32>>(size, &isOK);
	KxUtility::SetIfNotNull(isSuccess, isOK);

	if (isOK)
	{
		wxUString out;
		out.assign(buffer.data(), buffer.size());
		return out;
	}
	return wxEmptyString;
}
bool KxFileStream::WriteStringUTF32(const wxString& v)
{
	wxUString data(v);
	return OnSysWrite(data.data(), data.length() * sizeof(wxChar32)) != 0;
}
