#include "stdafx.h"
#include "RunningSystemProcessStdIO.h"

namespace
{
	kxf::String ReadToString(wxPipeInputStream& stream)
	{
		using namespace kxf;

		String result;
		char buffer[std::numeric_limits<int16_t>::max()] = {};

		while (stream.CanRead())
		{
			if (stream.Read(buffer, std::size(buffer)).LastRead() != 0)
			{
				result += String(buffer, wxConvWhateverWorks, stream.LastRead());
			}
			else
			{
				break;
			}
		}
		return result;
	}
}

namespace kxf
{
	String RunningSystemProcessStdIO::ReadStdOut() const
	{
		if (m_OutStream)
		{
			return ReadToString(*m_OutStream);
		}
		return {};
	}
	String RunningSystemProcessStdIO::ReadStdErr() const
	{
		if (m_ErrorStream)
		{
			return ReadToString(*m_ErrorStream);
		}
		return {};
	}
}
