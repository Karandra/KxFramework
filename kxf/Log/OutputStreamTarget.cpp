#include "stdafx.h"
#include "OutputStreamTarget.h"

namespace kxf::Log
{
	void OutputStreamTarget::DoLogText(const wxString& logMessage)
	{
		auto utf8 = logMessage.ToUTF8();
		if (utf8.length() != 0)
		{
			m_Stream->WriteAll(utf8.data(), utf8.length());
			m_Stream->Write("\n", 1);
		}
		else
		{
			constexpr const char nullMessage[] = "<empty log message>\n";
			m_Stream->WriteAll(nullMessage, std::size(nullMessage) - 1);
		}
	}
	
	void OutputStreamTarget::Flush()
	{
		wxLog::Flush();
		m_Stream->Flush();
	}
}
