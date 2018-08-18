#include "KxStdAfx.h"
#include "KxFramework/KxXDocumentNode.h"
#include "KxFramework/KxUtility.h"

bool KxXDocumentNodeInternal::ParseBool(const wxString& value, bool defaultValue)
{
	bool isUnknown = false;
	bool result = KxUtility::StringToBool(value, &isUnknown);
	return isUnknown ? defaultValue : result;
}
int KxXDocumentNodeInternal::ExtractIndexFromName(wxString& elementName, const wxString& XPathDelimiter)
{
	long index = 1;
	if (!XPathDelimiter.IsEmpty())
	{
		int indexStart = elementName.Find(XPathDelimiter);
		if ((size_t)indexStart != wxString::npos)
		{
			if (elementName.Mid(indexStart + XPathDelimiter.Length()).ToCLong(&index))
			{
				elementName.Truncate(indexStart);
				if (index <= 0)
				{
					index = 1;
				}
			}
		}
	}
	return index;
}
