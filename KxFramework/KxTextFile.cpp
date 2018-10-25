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
	KxFileStream::Disposition disposition = append ? KxFileStream::Disposition::OpenExisting : KxFileStream::Disposition::CreateAlways;
	KxFileStream stream(sourceFile, KxFileStream::Access::Write, disposition, KxFileStream::Share::Read);
	if (stream.IsOk())
	{
		bool isSuccess = true;
		if (append)
		{
			stream.SkipToEnd();
			isSuccess = stream.WriteStringUTF8(GetEOL(newLineMode));
		}

		return isSuccess && stream.WriteStringUTF8(Translate(sData, newLineMode));
	}
	return false;
}
bool KxTextFile::WriteToFile(const wxString& sourceFile, const KxStringVector& data, wxTextFileType newLineMode, bool append)
{
	KxFileStream::Disposition disposition = append ? KxFileStream::Disposition::OpenExisting : KxFileStream::Disposition::CreateAlways;
	KxFileStream stream(sourceFile, KxFileStream::Access::Write, disposition, KxFileStream::Share::Read);
	if (stream.IsOk())
	{
		bool isSuccess = true;
		const wxChar* newLine = GetEOL(newLineMode);
		if (append)
		{
			stream.SkipToEnd();
			isSuccess = stream.WriteStringUTF8(newLine);
		}
		
		for (size_t i = 0; i < data.size() && isSuccess; i++)
		{
			isSuccess = stream.WriteStringUTF8(data[i]);
			if (i + 1 != data.size())
			{
				isSuccess = stream.WriteStringUTF8(newLine);
			}
		}
		return isSuccess;
	}
	return false;
}
