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
		wxEvtHandler* EventHandler = nullptr;
		wxEventType EventType = wxEVT_NULL;
		const wxString* Source = nullptr;
		const wxString* Destination = nullptr;
	};
	struct CallbackDataFolder
	{
		wxEvtHandler* EventHandler = nullptr;
		wxEventType EventType = wxEVT_NULL;
		const wxString* Source = nullptr;
		const wxString* Destination = nullptr;
		const wxString* Current = nullptr;
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
				 wxEvtHandler* eventHandler = nullptr
	)
	{
		const DWORD excludedFlags = FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_SPARSE_FILE;
		const DWORD requiredFlags = 0;
		const wxString excludedFolder1(".");
		const wxString excludedFolder2("..");

		// Search
		WIN32_FIND_DATAW info = {0};

		wxString seqrchQuery = source + L"\\*";
		HANDLE searchHandle = ::FindFirstFileW(seqrchQuery.wc_str(), &info);
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
		uint32_t value = ::GetFileAttributesW(source.wc_str());
		if (value != INVALID_FILE_ATTRIBUTES)
		{
			return ::SetFileAttributesW(destination.wc_str(), value);
		}
		return false;
	}
	void CopyFolderAux(const wxString& source, const wxString& destination, const wxString& filter, bool recurse, DWORD flags, CallbackDataFolder* data, BOOL* cancel, LPPROGRESS_ROUTINE callback = nullptr)
	{
		auto list = KxFile(source).Find(filter, KxFS_ALL, false);
		data->TotalSize += list.size();

		for (const auto& element: list)
		{
			if (recurse &&::GetFileAttributesW(element.wc_str()) & FILE_ATTRIBUTE_DIRECTORY)
			{
				wxString folder(element);
				folder.Replace(source, destination, false);
				::CreateDirectoryW(folder.wc_str(), nullptr);
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

				wxString sourcePath = Namespace_Win32File + element;
				wxString destinationPath = KxFile(fileDestination).GetFullPathNS();
				BOOL ret = CopyFileExW(sourcePath.wc_str(), destinationPath.wc_str(), callback, data, cancel, flags);
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
	DWORD length = ::GetFullPathNameW(filePath.wc_str(), 0, nullptr, nullptr);
	if (length)
	{
		wxString out;
		LPWSTR oldPathStart = nullptr;
		::GetFullPathNameW(filePath.wc_str(), length, wxStringBuffer(out, length), &oldPathStart);
		return out;
	}
	return filePath;
}
wxString KxFile::GetLongPathName(const wxString& filePath)
{
	DWORD length = ::GetLongPathNameW(filePath.wc_str(), nullptr, 0);
	if (length)
	{
		wxString out;
		::GetLongPathNameW(filePath.wc_str(), wxStringBuffer(out, length), length);
		return out;
	}
	return filePath;
}

wxString& KxFile::TrimPath(wxString& path)
{
	wxUniChar charToRemove(wxS('\\'));

	size_t left = 0;
	for (; left < path.length(); left++)
	{
		if (path[left] != charToRemove)
		{
			break;
		}
	}
	path.Remove(0, left);

	size_t right = path.length() - 1;
	for (; right != 0; right--)
	{
		if (path[right] != charToRemove)
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
	if (path.Length() >= 2 && path[1] == wxS(':'))
	{
		return KxFNS_WIN32_FILE;
	}

	size_t count = 0;
	KxFileNamespace namespaceType = KxFNS_NULL;

	if (path.Length() >= 10)
	{
		wxString prefix = path.Left(10);
		if (prefix == Namespace_Win32FileUNC && prefix[9] == wxS(':'))
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
	return KxFileStream(path, KxFileStream::Access::ReadAttributes, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Read).GetLength();
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
	return SetCurrentDirectoryW(cwd.wc_str());
}

wxString KxFile::GetLongPath(const wxString& shortName)
{
	wxString out;
	wxString path = Namespace_Win32File + shortName;
	GetLongPathNameW(path.wc_str(), wxStringBuffer(out, INT16_MAX), INT16_MAX);

	out.Replace(Namespace_Win32File, "", false);
	return out;
}

//////////////////////////////////////////////////////////////////////////
bool KxFile::CheckValidity() const
{
	LPCWSTR checkPattern = wxS("*?/:\"<>|"); // Don't check for "\"
	if (IsRelative())
	{
		return !m_Path.Contains(checkPattern);
	}
	else
	{
		// Check after drive letter
		return !(GetFullPath().substr(GetFullPath().Find(wxS(':')) + 1)).Contains(checkPattern);
	}
}
wxString KxFile::NormalizePath(const wxString& path)
{
	// Can be useful: http://stackoverflow.com/questions/18580945/need-clarification-for-converting-paths-into-long-unicode-paths-or-the-ones-star?rq=1
	// And this: http://pdh11.blogspot.ru/2009/05/pathcanonicalize-versus-what-it-says-on.html

	wxString pathCopy = path;
	KxFileNamespace namespaceType = RemoveNamespacePrefix(pathCopy);
	if (namespaceType == KxFNS_WIN32_FILE)
	{
		pathCopy = GetFullPathName(path);
	}
	pathCopy.Replace(wxS("/"), wxS("\\"), true);
	TrimPath(pathCopy);
	return pathCopy;
}
void KxFile::Init()
{
	if (CheckValidity())
	{
		m_Path = NormalizePath(m_Path);
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
wxString KxFile::GetPath() const
{
	return m_Path;
}
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
	GetShortPathNameW(GetFullPathNS().wc_str(), wxStringBuffer(out, INT16_MAX), INT16_MAX);
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

bool KxFile::IsRelative() const
{
	return !IsAbsolute();
}
bool KxFile::IsAbsolute() const
{
	return m_Path.length() >= 2 && m_Path[1] == wxS(':');
}

wxString KxFile::GetDrive() const
{
	wxString path = GetFullPath();

	int pos = path.Find(wxS(':'));
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
				out.Append(wxS('\\'));
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
		if (m_Path[folderStart+1] == wxS('\\'))
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
		next = m_Path.find(wxS('\\'), folderStart);
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
	size_t pos = m_Path.rfind(wxS('\\'));
	if (pos != wxString::npos)
	{
		if (IsFile())
		{
			size_t dotPos = m_Path.rfind(wxS('.'));
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
	size_t pos = m_Path.rfind(wxS('.'));
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
		fullName += wxS('.');
		fullName += ext;
	}
	return fullName;
}
KxFile::SplitPathData KxFile::SplitPath() const
{
	SplitPathData data;
	data.Drive = GetDrive();
	data.Folder = m_Path.AfterFirst(wxS(':')).BeforeLast(wxS('\\'));
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
			if (::GetDiskFreeSpaceExW(GetDrive().wc_str(), nullptr, &total, nullptr))
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
	return ::GetFileAttributesW(GetFullPathNS().wc_str());
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
		KxFileStream fileHandle(GetFullPathNS(), KxFileStream::Access::RW, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Exclusive);
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
			isSuccess = ::DeviceIoControl(fileHandle.GetHandle(), FSCTL_SET_COMPRESSION, &value, sizeof(USHORT), nullptr, 0, &bytesReturned, nullptr);
		}
	}
	else if (attribute & FILE_ATTRIBUTE_ENCRYPTED)
	{
		if (set)
		{
			isSuccess = ::EncryptFileW(GetFullPathNS().wc_str());
		}
		else
		{
			isSuccess = ::DecryptFileW(GetFullPathNS().wc_str(), 0);
		}
	}
	return ::SetFileAttributesW(GetFullPathNS().wc_str(), KxUtility::ModFlag(GetAttributes(), attribute, set)) && isSuccess;
}
bool KxFile::SetAttributes(uint32_t attributes)
{
	if (attributes != INVALID_FILE_ATTRIBUTES)
	{
		return ::SetFileAttributesW(GetFullPathNS().wc_str(), attributes);
	}
	return false;
}

bool KxFile::IsInUse() const
{
	return KxFileStream(GetFullPathNS(), KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Exclusive).IsOk();
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
	BOOL ret = GetBinaryTypeW(GetFullPathNS().wc_str(), &binaryType);
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
		HANDLE fileHandle = ::CreateFileW(GetFullPath().wc_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			HANDLE mapHandle = ::CreateFileMappingW(fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if (mapHandle)
			{
				void* mapAddress = ::MapViewOfFileEx(mapHandle, FILE_MAP_READ, 0, 0, 0, nullptr);
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
	KxFileStream::Flags streamFlags = IsFolder() ? KxFileStream::Flags::BackupSemantics : KxFileStream::Flags::Normal;
	KxFileStream stream(GetFullPathNS(), KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Everything, streamFlags);
	if (stream.IsOk())
	{
		BOOL isOK = FALSE;
		FILETIME fileTime = {0};
		if (type == KxFILETIME_CREATION)
		{
			isOK = ::GetFileTime(stream.GetHandle(), &fileTime, nullptr, nullptr);
		}
		else if (type == KxFILETIME_LAST_ACCESS)
		{
			isOK = ::GetFileTime(stream.GetHandle(), nullptr, &fileTime, nullptr);
		}
		else if (type == KxFILETIME_MODIFICATION)
		{
			isOK = ::GetFileTime(stream.GetHandle(), nullptr, nullptr, &fileTime);
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
bool KxFile::SetFileTime(const wxDateTime& time, KxFileTime type)
{
	if (time.IsValid())
	{
		KxFileStream::Flags streamFlags = IsFolder() ? KxFileStream::Flags::BackupSemantics : KxFileStream::Flags::Normal;
		KxFileStream stream(GetFullPathNS(), KxFileStream::Access::WriteAttributes, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Everything, streamFlags);
		if (stream.IsOk())
		{
			TIME_ZONE_INFORMATION timeZoneInfo = {0};
			bool noDST = ::GetTimeZoneInformation(&timeZoneInfo) == TIME_ZONE_ID_UNKNOWN;

			SYSTEMTIME systemTime = {0};
			time.ToUTC(noDST).GetAsMSWSysTime(&systemTime);

			FILETIME fileTime = {0};
			if (::SystemTimeToFileTime(&systemTime, &fileTime))
			{
				switch (type)
				{
					case KxFILETIME_CREATION:
					{
						return ::SetFileTime(stream.GetHandle(), &fileTime, nullptr, nullptr);
					}
					case KxFILETIME_LAST_ACCESS:
					{
						return ::SetFileTime(stream.GetHandle(), nullptr, &fileTime, nullptr);
					}
					case KxFILETIME_MODIFICATION:
					{
						return ::SetFileTime(stream.GetHandle(), nullptr, nullptr, &fileTime);
					}
				};
			}
		}
	}
	return false;
}
bool KxFile::SetFileTime(const wxDateTime& creationTime, const wxDateTime& modificationTime, const wxDateTime& lastAccessTime)
{
	if (creationTime.IsValid() && modificationTime.IsValid() && lastAccessTime.IsValid())
	{
		KxFileStream::Flags streamFlags = IsFolder() ? KxFileStream::Flags::BackupSemantics : KxFileStream::Flags::Normal;
		KxFileStream stream(GetFullPathNS(), KxFileStream::Access::WriteAttributes, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Everything, streamFlags);
		if (stream.IsOk())
		{
			TIME_ZONE_INFORMATION timeZoneInfo = {0};
			bool noDST = ::GetTimeZoneInformation(&timeZoneInfo) == TIME_ZONE_ID_UNKNOWN;

			SYSTEMTIME creationSystemTime = {0};
			SYSTEMTIME modificationSystemTime = {0};
			SYSTEMTIME lastAccessSystemTime = {0};
			creationTime.ToUTC(noDST).GetAsMSWSysTime(&creationSystemTime);
			modificationTime.ToUTC(noDST).GetAsMSWSysTime(&modificationSystemTime);
			lastAccessTime.ToUTC(noDST).GetAsMSWSysTime(&lastAccessSystemTime);

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

	LPPROGRESS_ROUTINE callback = HasEventHandler() ? CopyFileCallback : nullptr;
	return CopyFileExW(GetFullPathNS().wc_str(), destination.GetFullPathNS().wc_str(), callback, &data, &cancel, flags);
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

	LPPROGRESS_ROUTINE callback = HasEventHandler() ? CopyFileCallback : nullptr;
	if (callback)
	{
		MoveFileWithProgressW(GetFullPathNS().wc_str(), destination.GetFullPathNS().wc_str(), callback, &data, flags);
	}
	else
	{
		MoveFileExW(GetFullPathNS().wc_str(), destination.GetFullPathNS().wc_str(), flags);
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
		return ::DeleteFileW(GetFullPathNS().wc_str());
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

		LPPROGRESS_ROUTINE callback = HasEventHandler() ? CopyFolderCallback : nullptr;
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
		::SetFileAttributesW(GetFullPathNS().wc_str(), FILE_ATTRIBUTE_NORMAL);
		return RemoveDirectoryW(GetFullPathNS().wc_str());
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
				::SetFileAttributesW(path.wc_str(), FILE_ATTRIBUTE_NORMAL);
				isSuccess = ::DeleteFileW(path.wc_str());
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
				::SetFileAttributesW(path.wc_str(), FILE_ATTRIBUTE_NORMAL);
				isSuccess = ::RemoveDirectoryW(path.wc_str());
			}

			// Removing main folder
			if (removeRootFolder)
			{
				::SetFileAttributesW(GetFullPathNS().wc_str(), FILE_ATTRIBUTE_NORMAL);
				return RemoveDirectoryW(GetFullPathNS().wc_str());
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
		wxString fullPath = Namespace_Win32File + GetDrive() + wxS('\\');
		const auto& folders = GetFoldersArray();
		for (size_t i = 0; i < folders.size(); i++)
		{
			fullPath.Append(folders[i]).Append(wxS('\\'));
			isSuccess = CreateDirectoryW(fullPath.wc_str(), nullptr);
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

	bool isSuccess = MoveFileExW(GetFullPathNS().wc_str(), destination.GetFullPathNS().wc_str(), flags);
	return isSuccess;
}

//////////////////////////////////////////////////////////////////////////
bool KxFile::ShellOpen()
{
	return ::ShellExecuteW(nullptr, L"open", GetFullPath().wc_str(), nullptr, nullptr, SW_SHOWNORMAL);
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
