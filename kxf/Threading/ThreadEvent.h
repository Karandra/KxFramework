#pragma once
#include "kxf/EventSystem/Event.h"

namespace kxf::Threading
{
	template<class T>
	static void AssertThreadExitCode()
	{
		static_assert(std::is_pointer_v<T> || std::is_integral_v<T> || std::is_enum_v<T>);
	}
}

namespace kxf
{
	class ThreadEvent: public wxThreadEvent
	{
		public:
			KxEVENT_MEMBER(ThreadEvent, Execute);
			KxEVENT_MEMBER(ThreadEvent, Started);
			KxEVENT_MEMBER(ThreadEvent, Finished);

		private:
			wxThread::ExitCode m_ExitCode = nullptr;

		public:
			ThreadEvent() = default;

		public:
			ThreadEvent* Clone() const override
			{
				return new ThreadEvent(*this);
			}

			template<class T = wxThread::ExitCode>
			T GetExitCode() const
			{
				Threading::AssertThreadExitCode<T>();

				return static_cast<T>(m_ExitCode);
			}
			
			template<class T>
			void SetExitCode(T code)
			{
				Threading::AssertThreadExitCode<T>();

				m_ExitCode = static_cast<wxThread::ExitCode>(code);
			}
	};
}
