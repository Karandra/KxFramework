#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/textfile.h>

class KxTextFile: public wxTextFile
{
	public:
		static wxString ReadToString(const wxString& sourceFile, wxTextFileType newLineMode = wxTextFileType_Dos);
		static KxStringVector ReadToArray(const wxString& sourceFile, wxTextFileType newLineMode = wxTextFileType_Dos);
		
		static bool WriteToFile(const wxString& sourceFile, const wxString& sData, wxTextFileType newLineMode = wxTextFileType_Dos, bool append = false);
		static bool WriteToFile(const wxString& sourceFile, const KxStringVector& data, wxTextFileType newLineMode = wxTextFileType_Dos, bool append = false);
};
