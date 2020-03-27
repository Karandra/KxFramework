#include "KxStdAfx.h"
#include "FSPath.h"
#include "LegacyDrive.h"
#include "Kx/General/BinarySize.h"
#include <KxFramework/KxFileStream.h>
#include <shlwapi.h>
#include "Kx/General/UndefWindows.h"

namespace KxFramework
{
	template<class TFunc>
	FSPath CallWinAPIWithLengthPrecalc(const FSPath& filePath, TFunc&& func)
	{
		wxString path = filePath.GetFullPath(FSPathNamespace::Win32File);

		const DWORD length = func(path.wc_str(), nullptr, 0);
		if (length)
		{
			wxString result;
			func(path.wc_str(), wxStringBuffer(result, length), length);

			FSPath fsPath = std::move(result);
			fsPath.EnsureNamespaceSet(filePath.GetNamespace());
			return fsPath;
		}
		return {};
	}

	uint32_t GetFileAttributes(const FSPath& filePath)
	{
		wxString path = filePath.GetFullPathWithNS(FSPathNamespace::Win32File);
		return ::GetFileAttributesW(path.wc_str());
	}
}

namespace KxFramework::FileSystem
{
	FSPath GetTempPath(const FSPath& rootDirectory)
	{
		FSPath fsPath = wxFileName::CreateTempFileName(rootDirectory.GetFullPath());
		fsPath.EnsureNamespaceSet(rootDirectory.GetNamespace());
		return fsPath;
	}
	FSPath GetFullPathName(const FSPath& filePath)
	{
		wxString path = filePath.GetFullPath(FSPathNamespace::Win32File);

		const DWORD length = ::GetFullPathNameW(path.wc_str(), 0, nullptr, nullptr);
		if (length)
		{
			wxString result;
			LPWSTR oldPathStart = nullptr;
			::GetFullPathNameW(path.wc_str(), length, wxStringBuffer(result, length), &oldPathStart);

			FSPath fsPath = std::move(result);
			fsPath.EnsureNamespaceSet(filePath.GetNamespace());
			return fsPath;
		}
		return {};
	}
	FSPath GetLongPathName(const FSPath& filePath)
	{
		return CallWinAPIWithLengthPrecalc(filePath, ::GetLongPathNameW);
	}
	FSPath GetShortPathName(const FSPath& filePath)
	{
		return CallWinAPIWithLengthPrecalc(filePath, ::GetShortPathNameW);
	}
	FSPath AbbreviatePath(const FSPath& filePath, int maxCharacters)
	{
		if (maxCharacters > 0)
		{
			if (maxCharacters < filePath.GetPathLength())
			{
				maxCharacters++;

				wxString source = filePath.GetFullPath();
				wxString result;
				::PathCompactPathExW(wxStringBuffer(result, maxCharacters), source.wc_str(), maxCharacters, 0);

				// Reinsert namespace information
				FSPath fsPath = std::move(result);
				fsPath.EnsureNamespaceSet(filePath.GetNamespace());
				return fsPath;
			}
			else
			{
				return filePath;
			}
		}
		return FSPath().SetNamespace(filePath.GetNamespace());
	}

	BinarySize GetFileSize(const FSPath& filePath)
	{
		return KxFileStream(filePath, KxFileStream::Access::ReadAttributes, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Everything).GetLength();
	}

	bool DoesExist(const FSPath& filePath)
	{
		return GetFileAttributes(filePath) != INVALID_FILE_ATTRIBUTES;
	}
	bool DoesFileExist(const FSPath& filePath)
	{
		const uint32_t attributes = GetFileAttributes(filePath);
		return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
	}
	bool DoesDirectoryExist(const FSPath& filePath)
	{
		const uint32_t attributes = GetFileAttributes(filePath);
		return attributes != INVALID_FILE_ATTRIBUTES && attributes & FILE_ATTRIBUTE_DIRECTORY;
	}
}
