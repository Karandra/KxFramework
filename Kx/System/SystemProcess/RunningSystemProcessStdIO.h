#pragma once
#include "ISystemProcessStdIO.h"
#include <wx/private/pipestream.h>
#include <wx/private/streamtempinput.h>

namespace KxFramework
{
	class KX_API RunningSystemProcessStdIO: public ISystemProcessStdIO
	{
		protected:
			std::unique_ptr<wxPipeOutputStream> m_InStream = nullptr;
			std::unique_ptr<wxPipeInputStream> m_OutStream = nullptr;
			std::unique_ptr<wxPipeInputStream> m_ErrorStream = nullptr;

		public:
			RunningSystemProcessStdIO() = default;

		public:
			wxOutputStream& GetStdIn() override
			{
				return *m_InStream;
			}
			wxInputStream& GetStdOut() override
			{
				return *m_OutStream;
			}
			wxInputStream& GetStdErr() override
			{
				return *m_ErrorStream;
			}

			String ReadStdOut() const override;
			String ReadStdErr() const override;
	};
}