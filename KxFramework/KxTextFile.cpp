#include "KxStdAfx.h"
#include "KxFramework/KxTextFile.h"
#include "KxFramework/KxFileStream.h"

wxString KxTextFile::ReadToString(const wxString& sourceFile, wxTextFileType newLineMode)
{
	wxTextFile fileHandle(sourceFile);
	fileHandle.Open();

	const wxChar* newLine = GetEOL(newLineMode);
	wxString text;
	for (size_t i = 0; i < fileHandle.GetLineCount(); i++)
	{
		text.Append(fileHandle.GetLine(i));
		if (i + 1 != fileHandle.GetLineCount())
		{
			text.Append(newLine);
		}
	}
	fileHandle.Close();
	return text;
}
KxStringVector KxTextFile::ReadToArray(const wxString& sourceFile, wxTextFileType newLineMode)
{
	wxTextFile fileHandle(sourceFile);
	fileHandle.Open();

	KxStringVector list;
	for (size_t i = 0; i < fileHandle.GetLineCount(); i++)
	{
		list.push_back(fileHandle.GetLine(i));
	}
	fileHandle.Close();
	return list;
}

bool KxTextFile::WriteToFile(const wxString& sourceFile, const wxString& sData, wxTextFileType newLineMode, bool append)
{
	KxFileStream file(sourceFile, KxFS_ACCESS_WRITE, append ? KxFS_DISP_OPEN_EXISTING : KxFS_DISP_CREATE_ALWAYS, KxFS_SHARE_READ);
	if (file.IsOk())
	{
		bool ret = true;
		if (append)
		{
			file.Seek(0, KxFS_SEEK_END);
			ret = file.WriteStringUTF8(GetEOL(newLineMode));
		}

		return ret && file.WriteStringUTF8(Translate(sData, newLineMode));
	}
	return false;
}
bool KxTextFile::WriteToFile(const wxString& sourceFile, const KxStringVector& data, wxTextFileType newLineMode, bool append)
{
	KxFileStream file(sourceFile, KxFS_ACCESS_WRITE, append ? KxFS_DISP_OPEN_EXISTING : KxFS_DISP_CREATE_ALWAYS, KxFS_SHARE_READ);
	if (file.IsOk())
	{
		bool ret = true;
		const wxChar* newLine = GetEOL(newLineMode);
		if (append)
		{
			file.Seek(0, KxFS_SEEK_END);
			ret = file.WriteStringUTF8(newLine);
		}
		
		for (size_t i = 0; i < data.size() && ret; i++)
		{
			ret = file.WriteStringUTF8(data[i]);
			if (i + 1 != data.size())
			{
				ret = file.WriteStringUTF8(newLine);
			}
		}
		return ret;
	}
	return false;
}
