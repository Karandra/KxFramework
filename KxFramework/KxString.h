#pragma once
#include "KxFramework/KxFramework.h"

class KxString
{
	private:
		KxString() = delete;
		KxString& operator=(KxString&) = delete;

	public:
		/* Upper/Lower */
		static wxUniChar& MakeLower(wxUniChar& c);
		static wxUniChar& MakeUpper(wxUniChar& c);

		static wxUniChar ToLower(const wxUniChar& c)
		{
			wxUniChar temp(c);
			MakeLower(temp);
			return temp;
		}
		static wxUniChar ToLower(const wxUniCharRef& c)
		{
			return ToLower(wxUniChar(c));
		}
		static wxUniChar ToUpper(const wxUniChar& c)
		{
			wxUniChar temp(c);
			MakeUpper(temp);
			return temp;
		}
		static wxUniChar ToUpper(const wxUniCharRef& c)
		{
			return ToUpper(wxUniChar(c));
		}

		static wxString& MakeLower(wxString& s);
		static wxString& MakeUpper(wxString& s);

		static wxString ToLower(const wxString& s)
		{
			wxString temp(s);
			MakeLower(temp);
			return temp;
		}
		static wxString ToUpper(const wxString& s)
		{
			wxString temp(s);
			MakeUpper(temp);
			return temp;
		}

		static wxString& MakeCapitalized(wxString& s, bool fistCharOnly = false);
		static wxString Capitalize(const wxString& s, bool fistCharOnly = false)
		{
			wxString temp(s);
			MakeCapitalized(temp);
			return temp;
		}

		/* Split */
		static KxStringVector Split(const wxString& source, size_t partLength, bool allowEmpty = false);
		static KxStringVector Split(const wxString& source, const wxString& pattern, bool allowEmpty = false);

		/* Join */
		static wxString Join(const KxStringVector& stringList, const wxString& separator = wxEmptyString);

		/* Searching and replacing */
		static size_t Find(const wxString& source, const wxString& pattern, size_t startAt = 0, bool caseSensetivity = true);
		static size_t Replace(wxString& source, const wxString& pattern, const wxString& replacement, size_t startAt = 0, bool caseSensetivity = true);
		
		/* Misc */
		static wxString AbbreviateFilePath(const wxString& source, size_t maxChars);

		static wxString& Trim(wxString& source, bool left, bool right)
		{
			if (left)
			{
				source.Trim(false);
			}
			if (right)
			{
				source.Trim(true);
			}
			return source;
		}
		static wxString Trim(const wxString& source, bool left, bool right)
		{
			wxString temp(source);
			Trim(temp, left, right);
			return temp;
		}
};
