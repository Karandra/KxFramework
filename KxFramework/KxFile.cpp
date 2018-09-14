#include "KxStdAfx.h"
#include "KxFramework/KxFile.h"
#include "KxFramework/KxFileStream.h"
#include "KxFramework/KxFileOperationEvent.h"
#include "KxFramework/KxFileFinder.h"
#include "KxFramework/KxShellMenu.h"
#include "KxFramework/KxShell.h"
#include "KxFramework/KxString.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxSystemAPI.h"
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <Shldisp.h>
#include <ShlObj.h>
#include "KxFramework/KxWinUndef.h"

#pragma comment(lib, "ShlWAPI.lib")
#pragma comment(lib, "Version.lib")

namespace
{
	const wxString Namespace_Null = wxEmptyString;
	const wxString Namespace_Win32File = L"\\\\?\\";
	const wxString Namespace_Win32Device = L"\\\\.\\";
	const wxString Namespace_NT = L"\\";
	const wxString Namespace_Win32FileUNC = L"\\\\?\\UNC\\";
	const wxString Namespace_Win32Volume = L"\\\\?\\Volume";
	const wxString Namespace_Network = L"\\\\";
	const wxString Namespace_NetworkUNC = L"\\\\?\\UNC\\";

	struct CallbackData
	{
		wxEvtHandler* EventHandler = NULL;
		wxEventType EventType = wxEVT_NULL;
		const wxString* Source = NULL;
		const wxString* Destination = NULL;
	};
	struct CallbackDataFolder
	{
		wxEvtHandler* EventHandler = NULL;
		wxEventType EventType = wxEVT_NULL;
		const wxString* Source = NULL;
		const wxString* Destination = NULL;
		const wxString* Current = NULL;
		uint64_t ProcessedSize = 0;
		uint64_t TotalSize = 0;
	};

	DWORD CALLBACK CopyFileCallback
	(
		LARGE_INTEGER TotalFileSize,
		LARGE_INTEGER TotalBytesTransferred,
		LARGE_INTEGER StreamSize,
		LARGE_INTEGER StreamBytesTransferred,
		DWORD dwStreamNumber,
		DWORD dwCallbackReason,
		HANDLE hSourceFile,
		HANDLE 	hDestinationFile,
		LPVOID lpData
	)
	{
		CallbackData* data = (CallbackData*)lpData;
		if (data && data->EventHandler)
		{
			KxFileOperationEvent event(data->EventType);
			event.SetEventObject(data->EventHandler);
			event.SetMajorProcessed(TotalBytesTransferred.QuadPart);
			event.SetMajorTotal(TotalFileSize.QuadPart);
			event.SetSource(*data->Source);
			event.SetCurrent(event.GetSource());
			event.SetDestination(*data->Destination);

			data->EventHandler->SafelyProcessEvent(event);
			if (event.IsAllowed())
			{
				return PROGRESS_CONTINUE;
			}
		}
		return PROGRESS_CANCEL;
	}
	DWORD CALLBACK CopyFolderCallback
	(
		LARGE_INTEGER TotalFileSize,
		LARGE_INTEGER TotalBytesTransferred,
		LARGE_INTEGER StreamSize,
		LARGE_INTEGER StreamBytesTransferred,
		DWORD dwStreamNumber,
		DWORD dwCallbackReason,
		HANDLE hSourceFile,
		HANDLE hDestinationFile,
		LPVOID lpData
	)
	{
		CallbackDataFolder* data = (CallbackDataFolder*)lpData;
		if (data && data->EventHandler)
		{
			KxFileOperationEvent event(data->EventType);
			event.SetEventObject(data->EventHandler);
			event.SetMinorProcessed(TotalBytesTransferred.QuadPart);
			event.SetMinorTotal(TotalFileSize.QuadPart);
			event.SetMajorProcessed(data->ProcessedSize);
			event.SetMajorTotal(data->TotalSize);
			event.SetSource(*data->Source);
			event.SetCurrent(*data->Current);
			event.SetDestination(*data->Destination);

			data->EventHandler->SafelyProcessEvent(event);
			if (event.IsAllowed())
			{
				return PROGRESS_CONTINUE;
			}
		}
		return PROGRESS_CANCEL;
	}

	void FindAux(KxStringVector& elementsList,
				 uint64_t* total,
				 const wxString& source,
				 const wxString& filter = KxFile::NullFilter,
				 KxFileSearchType elementType = KxFS_FILE,
				 bool recurse = false,
				 wxEvtHandler* eventHandler = NULL
	)
	{
		const DWORD excludedFlags = FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_SPARSE_FILE;
		const DWORD requiredFlags = 0;
		const wxString excludedFolder1(".");
		const wxString excludedFolder2("..");

		// Search
		WIN32_FIND_DATAW info = {0};
		HANDLE searchHandle = ::FindFirstFileW(source + L"\\*", &info);
		if (searchHandle != INVALID_HANDLE_VALUE)
		{
			ProcessElement:
			{
				bool isPass = true;
				switch (elementType)
				{
					case KxFS_FILE:
					{
						isPass = !(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
						break;
					}
					case KxFS_FOLDER:
					{
						isPass = info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
						break;
					}
					case KxFS_ALL:
					{
						isPass = true;
						break;
					}
					default:
					{
						// KxFS_FILE
						isPass = !(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
					}
				}

				wxString fileName(info.cFileName);
				if (isPass && !(info.dwFileAttributes & excludedFlags) && fileName != excludedFolder1 && fileName != excludedFolder2)
				{
					(*total)++;
					wxString currentFile = wxString::Format("%s\\%s", source, fileName);
					bool isMatch = fileName.Matches(filter);
					if (isMatch)
					{
						currentFile.Replace(Namespace_Win32File, "", false);
						elementsList.push_back(currentFile);
					}

					if (eventHandler)
					{
						KxFileOperationEvent event(KxEVT_FILEOP_SEARCH);
						event.SetEventObject(eventHandler);
						event.SetSource(source);
						event.SetMajorTotal(*total);
						event.SetCurrent(currentFile);
						event.SetMajorProcessed(elementsList.size());
						event.SetInt(isMatch);

						eventHandler->ProcessEvent(event);
						if (event.IsStopped())
						{
							return;
						}
					}
				}
			}

			while (::FindNextFileW(searchHandle, &info))
			{
				wxString fileName(info.cFileName);
				if (recurse && !(info.dwFileAttributes & excludedFlags) && (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && fileName != excludedFolder1 && fileName != excludedFolder2)
				{
					FindAux(elementsList, total, wxString::Format("%s\\%s", source, fileName), filter, elementType, recurse, eventHandler);
				}
				goto ProcessElement;
			}
			::FindClose(searchHandle);
		}
	}
	bool CopyAttributes(const wxString& source, const wxString& destination)
	{
		uint32_t value = ::GetFileAttributesW(source);
		if (value != INVALID_FILE_ATTRIBUTES)
		{
			return ::SetFileAttributesW(destination, value);
		}
		return false;
	}
	void CopyFolderAux(const wxString& source, const wxString& destination, const wxString& filter, bool recurse, DWORD flags, CallbackDataFolder* data, BOOL* cancel, LPPROGRESS_ROUTINE callback = NULL)
	{
		auto list = KxFile(source).Find(filter, KxFS_ALL, false);
		data->TotalSize += list.size();

		for (const auto& element: list)
		{
			if (recurse &&::GetFileAttributesW(element) & FILE_ATTRIBUTE_DIRECTORY)
			{
				wxString folder(element);
				folder.Replace(source, destination, false);
				::CreateDirectoryW(folder, NULL);
				CopyAttributes(Namespace_Win32File + element, Namespace_Win32File + folder);
				data->ProcessedSize++;

				CopyFolderAux(element, folder, filter, recurse, flags, data, cancel, callback);
			}
			else
			{
				data->Current = &element;
				wxString fileDestination(element);
				fileDestination.Replace(source, destination, false);
				data->ProcessedSize++;

				BOOL ret = CopyFileExW(Namespace_Win32File + element, KxFile(fileDestination).GetFullPathNS(), callback, data, cancel, flags);
				if (ret == FALSE && ::GetLastError() == ERROR_REQUEST_ABORTED)
				{
					return;
				}
			}
		}
	}
}

const wxString KxFile::NullFilter = L"*";

wxString KxFile::GetFullPathName(const wxString& filePath)
{
	DWORD length = ::GetFullPathNameW(filePath, NULL, NULL, NULL);
	if (length)
	{
		wxString out;
		LPWSTR oldPathStart = NULL;
		::GetFullPathNameW(filePath, length, wxStringBuffer(out, length), &oldPathStart);
		return out;
	}
	return filePath;
}
wxString KxFile::GetLongPathName(const wxString& filePath)
{
	DWORD length = ::GetLongPathNameW(filePath, NULL, 0);
	if (length)
	{
		wxString out;
		::GetLongPathNameW(filePath, wxStringBuffer(out, length), length);
		return out;
	}
	return filePath;
}

wxString& KxFile::TrimPath(wxString& path)
{
	wxUniChar c1('\\');
	wxUniChar c2('//');

	size_t left = 0;
	for (left; left < path.Length(); left++)
	{
		wxUniChar c = path[left];
		if (c != c1 && c != c2)
		{
			break;
		}
	}
	path.Remove(0, left);

	size_t right = path.Length() - 1;
	for (right; right > 0; right--)
	{
		wxUniChar c = path[right];
		if (c != c1 && c != c2)
		{
			break;
		}
	}
	path.Truncate(right + 1);

	return path;
}
const wxString& KxFile::GetNameSpaceString(KxFileNamespace namespaceType)
{
	switch (namespaceType)
	{
		case KxFNS_WIN32_FILE:
		{
			return Namespace_Win32File;
		}
		case KxFNS_WIN32_DEVICE:
		{
			return Namespace_Win32Device;
		}
		case KxFNS_NT:
		{
			return Namespace_NT;
		}
		case KxFNS_WIN32_FILE_UNC:
		{
			return Namespace_Win32FileUNC;
		}
		case KxFNS_WIN32_VOLUME:
		{
			return Namespace_Win32Volume;
		}
		case KxFNS_NETWORK:
		{
			return Namespace_Network;
		}
		case KxFNS_NETWORK_UNC:
		{
			return Namespace_NetworkUNC;
		}
	};
	return Namespace_Null;
}
KxFileNamespace KxFile::RemoveNamespacePrefix(wxString& path)
{
	if (path.Length() >= 2 && path[1] == L':')
	{
		return KxFNS_WIN32_FILE;
	}

	size_t count = 0;
	KxFileNamespace namespaceType = KxFNS_NULL;

	if (path.Length() >= 10)
	{
		wxString prefix = path.Left(10);
		if (prefix == Namespace_Win32FileUNC && prefix[9] == L':')
		{
			count = 8;
			namespaceType = KxFNS_WIN32_FILE_UNC;
		}
		else if (prefix == Namespace_Win32Volume)
		{
			count = 4;
			namespaceType = KxFNS_WIN32_VOLUME;
		}
	}
	if (path.Length() >= 8 && namespaceType == KxFNS_NULL)
	{
		wxString prefix = path.Left(8);
		if (prefix == Namespace_NetworkUNC)
		{
			count = 8;
			namespaceType = KxFNS_NETWORK_UNC;
		}
	}
	if (path.Length() >= 4 && namespaceType == KxFNS_NULL)
	{
		wxString prefix = path.Left(4);
		if (prefix == Namespace_Win32File)
		{
			namespaceType = KxFNS_WIN32_FILE;
			count = 4;
		}
		else if (prefix == Namespace_Win32Device)
		{
			namespaceType = KxFNS_WIN32_DEVICE;
			count = 4;
		}
	}
	if (path.Length() >= 2 && namespaceType == KxFNS_NULL)
	{
		wxString prefix = path.Left(2);
		if (prefix == Namespace_Network)
		{
			namespaceType = KxFNS_NETWORK;
			count = 2;
		}
	}
	if (path.Length() >= 1 && namespaceType == KxFNS_NULL)
	{
		if (path[0] == Namespace_NT[0])
		{
			namespaceType = KxFNS_NT;
			count = 1;
		}
	}

	path.Remove(0, count);
	if (namespaceType == KxFNS_NULL)
	{
		namespaceType = KxFNS_WIN32_FILE;
	}
	return namespaceType;
}
int64_t KxFile::GetFileSize(const wxString& path)
{
	return KxFileStream(path, KxFS_ACCESS_READ_ATTRIBUTES, KxFS_DISP_OPEN_EXISTING, KxFS_SHARE_READ).GetLength();
}
wxString KxFile::FormatFileSize(int64_t size, int precision, const wxString& failMassage)
{
	if (size == 0)
	{
		return "0 B";
	}
	return wxFileName::GetHumanReadableSize(size, failMassage, precision, wxSIZE_CONV_TRADITIONAL);
}
wxString KxFile::MakePath(const SplitPathData& data)
{
	return KxFile(data).GetFullPath();
}

wxString KxFile::GetCWD()
{
	wxString out;
	GetCurrentDirectoryW(INT16_MAX, wxStringBuffer(out, INT16_MAX));
	return out;
}
bool KxFile::SetCWD(const wxString& cwd)
{
	return SetCurrentDirectoryW(cwd);
}

wxString KxFile::GetLongPath(const wxString& shortName)
{
	wxString out;
	GetLongPathNameW(Namespace_Win32File + shortName, wxStringBuffer(out, INT16_MAX), INT16_MAX);

	out.Replace(Namespace_Win32File, "", false);
	return out;
}

//////////////////////////////////////////////////////////////////////////
bool KxFile::CheckValidity() const
{
	LPCWSTR checkPattern = L"*?/:\"<>|"; // Don't check for "\"
	if (IsRelative())
	{
		return !GetFullPath().Contains(checkPattern);
	}
	else
	{
		// Check after drive letter
		return !(GetFullPath().substr(GetFullPath().Find(':')+1)).Contains(checkPattern);
	}
}
wxString KxFile::NormalizePath(const wxString& path)
{
	// Can be useful: http://stackoverflow.com/questions/18580945/need-clarification-for-converting-paths-into-long-unicode-paths-or-the-ones-star?rq=1
	// And this: http://pdh11.blogspot.ru/2009/05/pathcanonicalize-versus-what-it-says-on.html

	wxString pathCopy = path;
	auto namespaceType = RemoveNamespacePrefix(pathCopy);
	if (namespaceType == KxFNS_WIN32_FILE)
	{
		pathCopy = GetFullPathName(path);
	}
	TrimPath(pathCopy);
	return path;
}
void KxFile::Init()
{
	if (CheckValidity())
	{
		NormalizePath(m_Path);
	}
	else
	{
		m_Path.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
KxFile::KxFile()
{
}
KxFile::KxFile(const SplitPathData& data)
{
	wxString drive = data.Drive;
	wxString folder = data.Folder;
	wxString fileName = data.FileName;
	wxString ext = data.Ext;

	if (drive.EndsWith("\\") != true)
	{
		drive.Append("\\");
	}

	if (folder.IsEmpty() != true)
	{
		folder = TrimPath(folder);
		if (fileName.IsEmpty() != true)
		{
			folder.Append("\\");
		}
	}

	if (fileName.IsEmpty() != true)
	{
		fileName = TrimPath(fileName);
	}

	if (ext.IsEmpty() != true && ext.StartsWith(".") != true)
	{
		ext.Prepend(".");
	}

	m_Path = drive + folder + fileName + ext;
	Init();
}
KxFile::KxFile(const wxString& fileName, KxFileNamespace namespaceType)
	:m_Path(fileName), m_NameSpace(namespaceType)
{
	Init();
}
KxFile::~KxFile()
{
}

//////////////////////////////////////////////////////////////////////////
// File names and parts
wxString KxFile::GetFullPath() const
{
	return GetFullPathName(m_Path);
}
wxString KxFile::GetFullPathNS() const
{
	return GetNameSpaceString(m_NameSpace) + GetFullPathName(m_Path);
}

wxString KxFile::GetShortPath() const
{
	wxString out;
	GetShortPathNameW(GetFullPathNS(), wxStringBuffer(out, INT16_MAX), INT16_MAX);
	out.Replace(Namespace_Win32File, "", false);
	return out;
}
wxString KxFile::GetLongPath() const
{
	return GetLongPath(m_Path);
}

wxString KxFile::AbbreviatePath(size_t max) const
{
	return KxString::AbbreviateFilePath(m_Path, max);
}

wxString KxFile::GetDrive() const
{
	const wxString& path = GetFullPath();

	int pos = path.Find(':');
	if (pos != wxNOT_FOUND)
	{
		return path.Left(pos+1);
	}
	return wxEmptyString;
}
wxString KxFile::GetFolders(int start, int end) const
{
	KxStringVector folders = GetFoldersArray();
	if (folders.empty() != true)
	{
		wxString out;
		size_t size = folders.size();
		if (end >= 0)
		{
			size = end;
		}
		for (size_t i = start; i < size; i++)
		{
			out.Append(folders[i]);
			if (i < size-1)
			{
				out.Append('\\');
			}
		}
		return out;
	}
	return wxEmptyString;
}
KxStringVector KxFile::GetFoldersArray() const
{
	KxStringVector folders;

	int folderStart = m_Path.Find(':');
	if (folderStart != wxNOT_FOUND)
	{
		if (m_Path[folderStart+1] == '\\')
		{
			folderStart += 2;
		}
	}
	else
	{
		folderStart = 0;
	}

	size_t next = (size_t)wxNOT_FOUND;
	do
	{
		next = m_Path.find('\\', folderStart);
		folders.push_back(m_Path.SubString(folderStart, next - 1));
		folderStart = next+1;
	}
	while (next != (size_t)wxNOT_FOUND);

	return folders;
}
wxString KxFile::GetFolder(int index) const
{
	KxStringVector folders = GetFoldersArray();
	int size = (int)folders.size();
	if (index < size)
	{
		if (index < 0)
		{
			index = size + index;
		}
		return folders[index];
	}
	return wxEmptyString;
}
wxString KxFile::GetName() const
{
	size_t pos = m_Path.rfind(L'\\');
	if (pos != wxString::npos)
	{
		if (IsFile())
		{
			size_t dotPos = m_Path.rfind(L'.');
			if (dotPos != wxString::npos)
			{
				return m_Path.SubString(pos + 1, dotPos - 1);
			}
		}
		return m_Path.Mid(pos + 1);
	}
	return wxEmptyString;
}
wxString KxFile::GetExt() const
{
	size_t pos = m_Path.rfind(L'.');
	if (pos != wxString::npos)
	{
		return m_Path.Mid(pos + 1);
	}
	return wxEmptyString;
}
wxString KxFile::GetFullName() const
{
	wxString fullName = GetName();
	wxString ext = GetExt();
	if (!ext.IsEmpty())
	{
		fullName += '.';
		fullName += ext;
	}
	return fullName;
}
KxFile::SplitPathData KxFile::SplitPath() const
{
	SplitPathData data;
	data.Drive = GetDrive();
	data.Folder = m_Path.AfterFirst(':').BeforeLast('\\');
	data.FileName = GetName();
	data.Ext = GetExt();
	return data;
}

wxString KxFile::CreateTempFile(const wxString& sRootPath)
{
	return wxFileName::CreateTempFileName(sRootPath);
}

//////////////////////////////////////////////////////////////////////////
int64_t KxFile::GetFileSize() const
{
	return GetFileSize(GetFullPathNS());
}
int64_t KxFile::GetFolderSize() const
{
	if (IsFolderExist())
	{
		if (GetFullPath().Length() <= _MAX_DRIVE)
		{
			ULARGE_INTEGER total = {0};
			if (::GetDiskFreeSpaceExW(GetDrive(), NULL, &total, NULL))
			{
				return total.QuadPart;
			}
			return -1;
		}
		else
		{
			uint64_t totalSize = 0;
			auto list = Find(NullFilter, KxFS_FILE, true);
			for (const auto& element: list)
			{
				int64_t size = GetFileSize(Namespace_Win32File + element);
				if (size != -1)
				{
					totalSize += size;
				}
			}
			return totalSize;
		}
	}
	return -1;
}
wxString KxFile::GetFormattedFileSize(int precision, const wxString& failMassage) const
{
	return FormatFileSize(GetFileSize(), precision, failMassage);
}
wxString KxFile::GetFormattedFolderSize(int precision, const wxString& failMassage) const
{
	return FormatFileSize(GetFolderSize(), precision, failMassage);
}

uint32_t KxFile::GetAttributes() const
{
	return ::GetFileAttributesW(GetFullPathNS());
}
bool KxFile::HasAttribute(uint32_t attribute) const
{
	uint32_t fileAttributes = GetAttributes();
	return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & attribute);
}
bool KxFile::SetAttribute(uint32_t attribute, bool set)
{
	BOOL isSuccess = TRUE;
	if (attribute & FILE_ATTRIBUTE_COMPRESSED)
	{
		KxFileStream fileHandle(GetFullPathNS(), KxFS_ACCESS_READ|KxFS_ACCESS_WRITE, KxFS_DISP_OPEN_EXISTING, KxFS_SHARE_EXCLUSIVE);
		if (fileHandle.IsOk())
		{
			USHORT value;
			if (set)
			{
				value = COMPRESSION_FORMAT_DEFAULT;
			}
			else
			{
				value = COMPRESSION_FORMAT_NONE;
			}
			DWORD bytesReturned = 0;
			isSuccess = ::DeviceIoControl(fileHandle.GetHandle(), FSCTL_SET_COMPRESSION, &value, sizeof(USHORT), NULL, 0, &bytesReturned, NULL);
		}
	}
	else if (attribute & FILE_ATTRIBUTE_ENCRYPTED)
	{
		if (set)
		{
			isSuccess = ::EncryptFileW(GetFullPathNS());
		}
		else
		{
			isSuccess = ::DecryptFileW(GetFullPathNS(), 0);
		}
	}
	return ::SetFileAttributesW(GetFullPathNS(), KxUtility::ModFlag(GetAttributes(), attribute, set)) && isSuccess;
}
bool KxFile::SetAttributes(uint32_t attributes)
{
	if (attributes != INVALID_FILE_ATTRIBUTES)
	{
		return ::SetFileAttributesW(GetFullPathNS(), attributes);
	}
	return false;
}

bool KxFile::IsInUse() const
{
	return KxFileStream(GetFullPathNS(), KxFS_ACCESS_READ, KxFS_DISP_OPEN_EXISTING, KxFS_SHARE_EXCLUSIVE).IsOk();
}
bool KxFile::IsFile() const
{
	return !HasAttribute(FILE_ATTRIBUTE_DIRECTORY);
}
bool KxFile::IsFolder() const
{
	return HasAttribute(FILE_ATTRIBUTE_DIRECTORY);
}

KxFileBinaryFormat KxFile::GetBinaryType() const
{
	DWORD binaryType = (DWORD)-1;
	BOOL ret = GetBinaryTypeW(GetFullPathNS(), &binaryType);
	DWORD lastError = GetLastError();

	KxFileBinaryFormat type = KxFBF_INVALID;
	if (ret && lastError != ERROR_BAD_EXE_FORMAT)
	{
		// Is this file is EXE
		switch (binaryType)
		{
			case SCS_WOW_BINARY:
			{
				type = KxFBF_WIN16;
				break;
			}
			case SCS_32BIT_BINARY:
			{
				type = KxFBF_WIN32;
				break;
			}
			case SCS_64BIT_BINARY:
			{
				type = KxFBF_WIN64;
				break;
			}
			case SCS_DOS_BINARY:
			{
				type = KxFBF_DOS;
				break;
			}
			case SCS_PIF_BINARY:
			{
				type = KxFBF_PIF;
				break;
			}
			case SCS_OS216_BINARY:
			{
				type = KxFBF_OS216;
				break;
			}
			case SCS_POSIX_BINARY:
			{
				type = KxFBF_POSIX;
				break;
			}
		};
	}
	else if (KxSystemAPI::ImageNtHeader)
	{
		// If this file is DLL
		HANDLE fileHandle = ::CreateFileW(GetFullPath(), GENERIC_READ, 0, NULL, OPEN_EXISTING, NULL, NULL);
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			HANDLE mapHandle = ::CreateFileMappingW(fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
			if (mapHandle)
			{
				void* mapAddress = ::MapViewOfFileEx(mapHandle, FILE_MAP_READ, 0, 0, 0, NULL);
				if (mapAddress)
				{
					IMAGE_NT_HEADERS* header = (IMAGE_NT_HEADERS*)KxSystemAPI::ImageNtHeader(mapAddress);
					if (header)
					{
						switch (header->FileHeader.Machine)
						{
							case IMAGE_FILE_MACHINE_I386:
							{
								type = KxFBF_WIN32;
								break;
							}
							case IMAGE_FILE_MACHINE_AMD64:
							{
								type = KxFBF_WIN64;
								break;
							}
							case IMAGE_FILE_MACHINE_IA64:
							{
								type = KxFBF_WIN64_IA;
								break;
							}
						}
					}
				}
			}
			::CloseHandle(fileHandle);
		}
	}

	return (KxFileBinaryFormat)type;
}
KxLibraryVersionInfo KxFile::GetVersionInfo() const
{
	return KxLibrary::GetVersionInfoFromFile(GetFullPath());
}

//////////////////////////////////////////////////////////////////////////
wxDateTime KxFile::GetFileTime(KxFileTime type) const
{
	KxFileStream stream(GetFullPathNS(), KxFS_ACCESS_READ, KxFS_DISP_OPEN_EXISTING, KxFS_SHARE_ALL, IsFolder() ? KxFS_FLAG_BACKUP_SEMANTICS : KxFS_FLAG_NORMAL);
	if (stream.IsOk())
	{
		BOOL isOK = FALSE;
		FILETIME fileTime = {0};
		if (type == KxFILETIME_CREATION)
		{
			isOK = ::GetFileTime(stream.GetHandle(), &fileTime, NULL, NULL);
		}
		else if (type == KxFILETIME_LAST_ACCESS)
		{
			isOK = ::GetFileTime(stream.GetHandle(), NULL, &fileTime, NULL);
		}
		else if (type == KxFILETIME_MODIFICATION)
		{
			isOK = ::GetFileTime(stream.GetHandle(), NULL, NULL, &fileTime);
		}

		FILETIME fileTimeLocal = {0};
		if (isOK && ::FileTimeToLocalFileTime(&fileTime, &fileTimeLocal))
		{
			SYSTEMTIME systemTime = {0};
			if (::FileTimeToSystemTime(&fileTimeLocal, &systemTime))
			{
				wxDateTime t;
				t.SetFromMSWSysTime(systemTime);

				return t;
			}
		}
	}
	return wxDefaultDateTime;
}
bool KxFile::SetFileTime(const wxDateTime& t, KxFileTime type)
{
	if (t.IsValid())
	{
		KxFileStream stream(GetFullPathNS(), KxFS_ACCESS_WRITE_ATTRIBUTES, KxFS_DISP_OPEN_EXISTING, KxFS_SHARE_ALL, IsFolder() ? KxFS_FLAG_BACKUP_SEMANTICS : KxFS_FLAG_NORMAL);
		if (stream.IsOk())
		{
			TIME_ZONE_INFORMATION timeZoneInfo = {0};
			bool noDST = ::GetTimeZoneInformation(&timeZoneInfo) == TIME_ZONE_ID_UNKNOWN;

			SYSTEMTIME systemTime = {0};
			t.ToUTC(noDST).GetAsMSWSysTime(&systemTime);

			FILETIME fileTime = {0};
			if (::SystemTimeToFileTime(&systemTime, &fileTime))
			{
				switch (type)
				{
					case KxFILETIME_CREATION:
					{
						return ::SetFileTime(stream.GetHandle(), &fileTime, NULL, NULL);
					}
					case KxFILETIME_LAST_ACCESS:
					{
						return ::SetFileTime(stream.GetHandle(), NULL, &fileTime, NULL);
					}
					case KxFILETIME_MODIFICATION:
					{
						return ::SetFileTime(stream.GetHandle(), NULL, NULL, &fileTime);
					}
				};
			}
		}
	}
	return false;
}
bool KxFile::SetFileTime(const wxDateTime& tCreation, const wxDateTime& tModification, const wxDateTime& tLastAccess)
{
	if (tCreation.IsValid() && tModification.IsValid() && tLastAccess.IsValid())
	{
		KxFileStream stream(GetFullPathNS(), KxFS_ACCESS_WRITE_ATTRIBUTES, KxFS_DISP_OPEN_EXISTING, KxFS_SHARE_ALL, IsFolder() ? KxFS_FLAG_BACKUP_SEMANTICS : KxFS_FLAG_NORMAL);
		if (stream.IsOk())
		{
			TIME_ZONE_INFORMATION timeZoneInfo = {0};
			bool noDST = ::GetTimeZoneInformation(&timeZoneInfo) == TIME_ZONE_ID_UNKNOWN;

			SYSTEMTIME creationSystemTime = {0};
			SYSTEMTIME modificationSystemTime = {0};
			SYSTEMTIME lastAccessSystemTime = {0};
			tCreation.ToUTC(noDST).GetAsMSWSysTime(&creationSystemTime);
			tModification.ToUTC(noDST).GetAsMSWSysTime(&modificationSystemTime);
			tLastAccess.ToUTC(noDST).GetAsMSWSysTime(&lastAccessSystemTime);

			FILETIME creationFileTime = {0};
			FILETIME modificationFileTime = {0};
			FILETIME lastAccessFileTime = {0};
			if (::SystemTimeToFileTime(&creationSystemTime, &creationFileTime) &&
				::SystemTimeToFileTime(&modificationSystemTime, &modificationFileTime) &&
				::SystemTimeToFileTime(&lastAccessSystemTime, &lastAccessFileTime))
			{
				return ::SetFileTime(stream.GetHandle(), &creationFileTime, &lastAccessFileTime, &modificationFileTime);
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool KxFile::IsFileExist() const
{
	return IsFilePathExist(GetFullPathNS());
}
bool KxFile::IsFolderExist() const
{
	return IsFolderPathExist(GetFullPathNS());
}
bool KxFile::IsExist() const
{
	return IsPathExist(GetFullPathNS());
}

//////////////////////////////////////////////////////////////////////////
KxStringVector KxFile::Find(const wxString& filter, KxFileSearchType elementType, bool recurse) const
{
	uint64_t total = 0;
	KxStringVector elementsList;
	FindAux(elementsList, &total, GetFullPathNS(), filter, elementType, recurse, m_EventHnadler);
	return elementsList;
}
KxStringVector KxFile::Find(const KxStringVector& filters, KxFileSearchType elementType, bool recurse) const
{
	KxStringVector allFound = Find(NullFilter, elementType, recurse);
	KxStringVector found;

	for (size_t i = 0; i < allFound.size(); i++)
	{
		wxString name = allFound[i].AfterLast('\\');
		for (const wxString& sMask: filters)
		{
			if (name.Matches(sMask))
			{
				found.emplace_back(allFound[i]);
				break;
			}
		}
	}
	return found;
}

//////////////////////////////////////////////////////////////////////////
bool KxFile::CopyFile(const KxFile& destination, bool overwrite)
{
	CallbackData data = {0};
	data.EventHandler = m_EventHnadler;
	data.EventType = KxEVT_FILEOP_COPY;
	data.Source = &GetFullPath();
	data.Destination = &destination.GetFullPath();

	BOOL cancel = FALSE;
	DWORD flags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION|COPY_FILE_COPY_SYMLINK;
	if (!overwrite)
	{
		flags |= COPY_FILE_FAIL_IF_EXISTS;
	}

	LPPROGRESS_ROUTINE callback = HasEventHandler() ? CopyFileCallback : NULL;
	return CopyFileExW(GetFullPathNS(), destination.GetFullPathNS(), callback, &data, &cancel, flags);
}
bool KxFile::MoveFile(const KxFile& destination, bool overwrite)
{
	bool isSuccess = false;

	CallbackData data = {0};
	data.EventHandler = m_EventHnadler;
	data.EventType = KxEVT_FILEOP_MOVE;
	data.Source = &GetFullPath();
	data.Destination = &destination.GetFullPath();

	DWORD flags = MOVEFILE_COPY_ALLOWED;
	if (overwrite)
	{
		flags |= MOVEFILE_REPLACE_EXISTING;
	}

	LPPROGRESS_ROUTINE callback = HasEventHandler() ? CopyFileCallback : NULL;
	if (callback)
	{
		MoveFileWithProgressW(GetFullPathNS(), destination.GetFullPathNS(), callback, &data, flags);
	}
	else
	{
		MoveFileExW(GetFullPathNS(), destination.GetFullPathNS(), flags);
	}
	return isSuccess;
}
bool KxFile::RemoveFile(bool toRecycleBin)
{
	if (HasEventHandler())
	{
		KxFileOperationEvent event(KxEVT_FILEOP_REMOVE);
		event.SetEventObject(m_EventHnadler);
		event.SetCurrent(GetFullPath());
		m_EventHnadler->ProcessEvent(event);

		if (!event.IsAllowed())
		{
			return false;
		}
	}

	if (toRecycleBin)
	{
		return KxShell::FileOperation(GetFullPath(), KxFS_FILE, KxFOF_DELETE, true);
	}
	else
	{
		SetAttributes(FILE_ATTRIBUTE_NORMAL);
		return ::DeleteFileW(GetFullPathNS());
	}
}

void KxFile::CopyFolder(const wxString& filter, const KxFile& destination, bool recurse, bool overwrite)
{
	if (!destination.IsFolderExist())
	{
		destination.CreateFolder();
		CopyAttributes(GetFullPathNS(), destination.GetFullPathNS());
	}

	if (IsFolderExist() && destination.IsFolderExist())
	{
		// Flags
		BOOL cancel = FALSE;
		DWORD flags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION|COPY_FILE_COPY_SYMLINK;
		if (!overwrite)
		{
			flags |= COPY_FILE_FAIL_IF_EXISTS;
		}

		// Initial state
		CallbackDataFolder data = {0};
		data.EventHandler = m_EventHnadler;
		data.EventType = KxEVT_FILEOP_COPY_FOLDER;
		data.Source = &GetFullPath();
		data.Destination = &destination.GetFullPath();
		data.ProcessedSize = 0;
		data.TotalSize = 0;

		LPPROGRESS_ROUTINE callback = HasEventHandler() ? CopyFolderCallback : NULL;
		CopyFolderAux(GetFullPath(), destination.GetFullPath(), filter, recurse, flags, &data, &cancel, callback);
	}
}
bool KxFile::RemoveFolder(bool removeRootFolder, bool toRecycleBin)
{
	if (HasEventHandler())
	{
		KxFileOperationEvent event(KxEVT_FILEOP_REMOVE_FOLDER);
		event.SetEventObject(m_EventHnadler);
		event.SetCurrent(GetFullPath());
		m_EventHnadler->ProcessEvent(event);

		if (!event.IsAllowed())
		{
			return false;
		}
	}

	if (toRecycleBin)
	{
		return KxShell::FileOperation(GetFullPath(), KxFS_FOLDER, KxFOF_DELETE, true, false);
	}
	else
	{
		::SetFileAttributesW(GetFullPathNS(), FILE_ATTRIBUTE_NORMAL);
		return RemoveDirectoryW(GetFullPathNS());
	}
}
bool KxFile::RemoveFolderTree(bool removeRootFolder, bool toRecycleBin)
{
	if (HasEventHandler())
	{
		KxFileOperationEvent event(KxEVT_FILEOP_REMOVE_FOLDER);
		event.SetEventObject(m_EventHnadler);
		event.SetCurrent(GetFullPath());
		m_EventHnadler->ProcessEvent(event);

		if (!event.IsAllowed())
		{
			return false;
		}
	}

	if (IsFolderExist())
	{
		if (toRecycleBin)
		{
			return KxShell::FileOperation(GetFullPath(), KxFS_FOLDER, KxFOF_DELETE, true, true);
		}
		else
		{
			// For tracking success of entire operation.
			// It won't correctly reflect status (since will be overwritten by last result), but at least something
			bool isSuccess = false;
			bool isFile = true;

			// Removing files
			uint64_t processed = 0;
			auto filesList = Find(NullFilter, KxFS_FILE, true);
			for (auto& fileName: filesList)
			{
				if (HasEventHandler())
				{
					KxFileOperationEvent event(KxEVT_FILEOP_REMOVE_FOLDER);
					event.SetEventObject(m_EventHnadler);
					event.SetSource(GetFullPath());
					event.SetCurrent(fileName);
					event.SetMajorTotal(filesList.size());
					event.SetMajorProcessed(processed);
					event.SetInt(isFile);
					m_EventHnadler->ProcessEvent(event);

					if (event.IsStopped())
					{
						return false;
					}
				}

				wxString path = fileName.Prepend(Namespace_Win32File);
				::SetFileAttributesW(path, FILE_ATTRIBUTE_NORMAL);
				isSuccess = ::DeleteFileW(path);
				processed++;
			}

			// Removing folders
			processed = 0;
			isFile = false;
			auto foldersList = Find(NullFilter, KxFS_FOLDER, true);
			for (auto i = foldersList.begin(); i != foldersList.end(); ++i)
			{
				if (HasEventHandler())
				{
					KxFileOperationEvent event(KxEVT_FILEOP_REMOVE_FOLDER);
					event.SetEventObject(m_EventHnadler);
					event.SetSource(GetFullPath());
					event.SetCurrent(*i);
					event.SetMajorTotal(foldersList.size());
					event.SetMajorProcessed(processed++);
					event.SetInt(isFile);
					m_EventHnadler->ProcessEvent(event);

					if (event.IsStopped())
					{
						return false;
					}
				}

				wxString path = i->Prepend(Namespace_Win32File);
				::SetFileAttributesW(path, FILE_ATTRIBUTE_NORMAL);
				isSuccess = ::RemoveDirectoryW(path);
			}

			// Removing main folder
			if (removeRootFolder)
			{
				::SetFileAttributesW(GetFullPathNS(), FILE_ATTRIBUTE_NORMAL);
				return RemoveDirectoryW(GetFullPathNS());
			}
			else
			{
				return isSuccess;
			}
		}
	}
	return false;
}
bool KxFile::CreateFolder() const
{
	if (IsFolderExist())
	{
		return true;
	}
	else
	{
		BOOL isSuccess = false;
		wxString fullPath = Namespace_Win32File + GetDrive() + "\\";
		const auto& folders = GetFoldersArray();
		for (size_t i = 0; i < folders.size(); i++)
		{
			fullPath.Append(folders[i]).Append("\\");
			isSuccess = CreateDirectoryW(fullPath, NULL);
		}
		return isSuccess;
	}
}
bool KxFile::Rename(const KxFile& destination, bool overwrite)
{
	if (HasEventHandler())
	{
		KxFileOperationEvent event(KxEVT_FILEOP_RENAME);
		event.SetEventObject(m_EventHnadler);
		event.SetSource(GetFullPath());
		event.SetDestination(destination.GetFullPath());
		event.SetInt(overwrite);
		m_EventHnadler->ProcessEvent(event);

		if (!event.IsAllowed())
		{
			return false;
		}
	}

	DWORD flags = 0;
	if (overwrite)
	{
		flags |= MOVEFILE_REPLACE_EXISTING;
	}

	bool isSuccess = MoveFileExW(GetFullPathNS(), destination.GetFullPathNS(), flags);
	return isSuccess;
}

//////////////////////////////////////////////////////////////////////////
bool KxFile::ShellOpen()
{
	return ::ShellExecuteW(NULL, L"open", GetFullPath(), NULL, NULL, SW_SHOWNORMAL);
}

//////////////////////////////////////////////////////////////////////////
bool KxFile::operator==(const KxFile& other)
{
	return KxString::ToLower(GetFullPath()) == KxString::ToLower(other.GetFullName());
}
bool KxFile::operator!=(const KxFile& other)
{
	return !(*this == other);
}
