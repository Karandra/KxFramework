#pragma once
#include "../Common.h"
#include "Kx/EventSystem/Event.h"

namespace KxFramework
{
	class KX_API ProcessEvent: public wxNotifyEvent
	{
		public:
			KxEVENT_MEMBER(ProcessEvent, InputIdle);
			KxEVENT_MEMBER(ProcessEvent, Termination);

		private:
			uint32_t m_PID = 0;
			uint32_t m_ExitCode = std::numeric_limits<uint32_t>::max();

		public:
			ProcessEvent(EventID type = Event::EvtNull, int id = wxID_ANY)
				:wxNotifyEvent(type, id)
			{
			}
			ProcessEvent(EventID type, uint32_t pid, uint32_t exitCode)
				:wxNotifyEvent(type, wxID_ANY), m_PID(pid), m_ExitCode(exitCode)
			{
			}

		public:
			ProcessEvent* Clone() const override
			{
				return new ProcessEvent(*this);
			}

			uint32_t GetPID() const
			{
				return m_PID;
			}
			void SetPID(uint32_t pid)
			{
				m_PID = static_cast<int>(pid);
			}

			uint32_t GetExitCode() const
			{
				return m_ExitCode;
			}
			void SetExitCode(uint32_t pid)
			{
				m_ExitCode = static_cast<int>(pid);
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(ProcessEvent);
	};
}
