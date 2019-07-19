#pragma once
#include "Kx/Core/EventSystem/Event.h"

namespace Kx::Threading
{
	template<class T> static void AssertThreadExitCode()
	{
		static_assert(std::is_pointer_v<T> || std::is_integral_v<T> || std::is_enum_v<T>);
	}
}

class KxThreadEvent: public wxThreadEvent
{
	public:
		KxEVENT_MEMBER(KxThreadEvent, Execute);
		KxEVENT_MEMBER(KxThreadEvent, Started);
		KxEVENT_MEMBER(KxThreadEvent, Finished);

	private:
		wxThread::ExitCode m_ExitCode = 0;

	public:
		KxThreadEvent() = default;

	public:
		KxThreadEvent* Clone() const override
		{
			return new KxThreadEvent(*this);
		}

		template<class T = wxThread::ExitCode> T GetExitCode() const
		{
			Kx::Threading::AssertThreadExitCode<T>();
			return static_cast<T>(m_ExitCode);
		}
		template<class T> void SetExitCode(T code)
		{
			Kx::Threading::AssertThreadExitCode<T>();
			m_ExitCode = static_cast<wxThread::ExitCode>(code);
		}
};
