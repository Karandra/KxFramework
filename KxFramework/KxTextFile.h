/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/textfile.h>

class KX_API KxTextFile: public wxTextFile
{
	public:
		static wxString ReadToString(const wxString& sourceFile, wxTextFileType newLineMode = wxTextFileType_Dos);
		static KxStringVector ReadToArray(const wxString& sourceFile, wxTextFileType newLineMode = wxTextFileType_Dos);
		
		static bool WriteToFile(const wxString& sourceFile, const wxString& sData, wxTextFileType newLineMode = wxTextFileType_Dos, bool append = false);
		static bool WriteToFile(const wxString& sourceFile, const KxStringVector& data, wxTextFileType newLineMode = wxTextFileType_Dos, bool append = false);
};
