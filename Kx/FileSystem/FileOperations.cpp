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
	wxString CallWinAPIWithLengthPrecalc(const FSPath& filePath, TFunc&& func)
	{
		wxString path = filePath.GetFullPath(FSPathNamespace::Win32File);

		const DWORD length = func(path.wc_str(), nullptr, 0);
		if (length)
		{
			wxString out;
			func(path.wc_str(), wxStringBuffer(out, length), length);
			return out;
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
		if (!fsPath.HasNamespace())
		{
			fsPath.SetNamespace(rootDirectory.GetNamespace());
		}
		return fsPath;
	}
	FSPath GetFullPathName(const FSPath& filePath)
	{
		wxString path = filePath.GetFullPath(FSPathNamespace::Win32File);

		const DWORD length = ::GetFullPathNameW(path.wc_str(), 0, nullptr, nullptr);
		if (length)
		{
			wxString out;
			LPWSTR oldPathStart = nullptr;
			::GetFullPathNameW(path.wc_str(), length, wxStringBuffer(out, length), &oldPathStart);
			return out;
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
				FSPath fsPath(std::move(result));
				if (!fsPath.HasNamespace())
				{
					fsPath.SetNamespace(filePath.GetNamespace());
				}
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
