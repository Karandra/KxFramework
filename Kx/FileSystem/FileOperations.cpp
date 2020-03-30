#include "KxStdAfx.h"
#include "FileOperations.h"
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
}

namespace KxFramework::FileSystem
{
	FSPath CreateTempPathName(const FSPath& rootDirectory)
	{
		FSPath fsPath = wxFileName::CreateTempFileName(rootDirectory.GetFullPath());
		fsPath.EnsureNamespaceSet(rootDirectory.GetNamespace());
		return fsPath;
	}
	FSPath GetFullPathName(const FSPath& path)
	{
		wxString pathString = path.GetFullPath(FSPathNamespace::Win32File);

		const DWORD length = ::GetFullPathNameW(pathString.wc_str(), 0, nullptr, nullptr);
		if (length)
		{
			wxString result;
			LPWSTR oldPathStart = nullptr;
			::GetFullPathNameW(pathString.wc_str(), length, wxStringBuffer(result, length), &oldPathStart);

			FSPath fsPath = std::move(result);
			fsPath.EnsureNamespaceSet(path.GetNamespace());
			return fsPath;
		}
		return {};
	}
	FSPath GetLongPathName(const FSPath& path)
	{
		return CallWinAPIWithLengthPrecalc(path, ::GetLongPathNameW);
	}
	FSPath GetShortPathName(const FSPath& path)
	{
		return CallWinAPIWithLengthPrecalc(path, ::GetShortPathNameW);
	}
	FSPath AbbreviatePath(const FSPath& path, int maxCharacters)
	{
		if (maxCharacters > 0)
		{
			const size_t sourceLength = path.GetPathLength();
			if (maxCharacters < sourceLength && sourceLength < MAX_PATH)
			{
				maxCharacters++;

				wxString source = path.GetFullPath();
				wxString result;
				::PathCompactPathExW(wxStringBuffer(result, maxCharacters), source.wc_str(), maxCharacters, 0);

				return FSPath(std::move(result)).SetNamespace(path.GetNamespace());
			}
			else if (sourceLength >= MAX_PATH)
			{
				// PathCompactPathEx doesn't work for paths longer than 'MAX_PATH'
				// Taking Jeff Atwood 's idea but with a bit different RegEx.
				// Doesn't guarantees final length to not exceed 'maxCharacters'.
				// https://blog.codinghorror.com/shortening-long-file-paths/

				wxString source = path.GetFullPath();
				wxRegEx regEx(wxS(R"((\w+:\\|)([^\\]+[^\\]+).*\\([^\\]+))"), wxRE_ADVANCED|wxRE_ICASE);
				if (regEx.Matches(source))
				{
					regEx.ReplaceAll(&source, wxS(R"(\1\2\\...\\\3)"));

					// If it's still longer just truncate it and add ellipsis
					if (source.length() > maxCharacters)
					{
						source.RemoveLast(source.length() - maxCharacters - 3);
						source += wxS("...");
					}
					return FSPath(std::move(source)).SetNamespace(path.GetNamespace());
				}
			}
			return path;
		}
		return FSPath().SetNamespace(path.GetNamespace());
	}
}
