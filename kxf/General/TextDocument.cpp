#include "stdafx.h"
#include "TextDocument.h"
#include "kxf/FileSystem/FileStream.h"
#include "kxf/Utility/CallAtScopeExit.h"
#include <wx/textfile.h>

namespace
{
	wxTextFileType MapLineBreakType(kxf::TextDocument::LineBreakFormat lineBreakFormat) noexcept
	{
		using namespace kxf::TextDocument;

		switch (lineBreakFormat)
		{
			case LineBreakFormat::Windows:
			{
				return wxTextFileType::wxTextFileType_Dos;
			}
			case LineBreakFormat::Unix:
			{
				return wxTextFileType::wxTextFileType_Unix;
			}
			case LineBreakFormat::Mac:
			{
				return wxTextFileType::wxTextFileType_Mac;
			}
		}
		return wxTextFileType::wxTextFileType_None;
	}
}

namespace kxf::TextDocument
{
	size_t Read(const FSPath& filePath, std::function<bool(String)> func)
	{
		wxTextFile textFile(filePath.GetFullPath());
		if (textFile.Open())
		{
			Utility::CallAtScopeExit atExit([&]()
			{
				textFile.Close();
			});

			size_t count = 0;
			for (size_t i = 0; i < textFile.GetLineCount(); i++)
			{
				count++;
				if (!std::invoke(func, String(textFile.GetLine(i))))
				{
					break;
				}
			}
			return count;
		}
		return 0;
	}
	String Read(const FSPath& filePath, LineBreakFormat lineBreakFormat)
	{
		String result;
		const wxChar* lineBreak = wxTextFile::GetEOL(MapLineBreakType(lineBreakFormat));

		Read(filePath, [&](String line)
		{
			if (!result.IsEmpty())
			{
				result += lineBreak;
			}
			result += std::move(line);

			return true;
		});
		return result;
	}

	bool Write(const FSPath& filePath, const String& text, bool append, LineBreakFormat lineBreakFormat)
	{
		const FileStreamDisposition disposition = append ? FileStreamDisposition::OpenExisting : FileStreamDisposition::CreateAlways;
		FileStream stream(filePath, FileStreamAccess::Write, disposition, FileStreamShare::Read);
		if (stream)
		{
			bool isSuccess = true;
			const wxTextFileType lineBreakFormatWx = MapLineBreakType(lineBreakFormat);

			if (append)
			{
				stream.SkipToEnd();
				isSuccess = stream.WriteStringUTF8(wxTextFile::GetEOL(lineBreakFormatWx));
			}

			return isSuccess && stream.WriteStringUTF8(wxTextFile::Translate(text, lineBreakFormatWx));
		}
		return false;
	}
	bool Write(const FSPath& filePath, std::function<String()> func, bool append, LineBreakFormat lineBreakFormat)
	{
		const FileStreamDisposition disposition = append ? FileStreamDisposition::OpenExisting : FileStreamDisposition::CreateAlways;
		FileStream stream(filePath, FileStreamAccess::Write, disposition, FileStreamShare::Read);
		if (stream)
		{
			const wxTextFileType lineBreakFormatWx = MapLineBreakType(lineBreakFormat);
			const wxChar* lineBreak = wxTextFile::GetEOL(lineBreakFormatWx);

			bool isSuccess = true;
			if (append)
			{
				stream.SkipToEnd();
				isSuccess = stream.WriteStringUTF8(lineBreak);
			}

			while (isSuccess)
			{
				String text = std::invoke(func);
				if (!text.IsEmpty())
				{
					isSuccess = stream.WriteStringUTF8(wxTextFile::Translate(text, lineBreakFormatWx));
					if (isSuccess)
					{
						isSuccess = stream.WriteStringUTF8(lineBreak);
					}
				}
				else
				{
					break;
				}
			}
			return isSuccess;
		}
		return false;
	}
}
