#pragma once
#include "Kx/EventSystem/Event.h"

namespace KxFramework::Threading
{
	template<class T>
	static void AssertThreadExitCode()
	{
		static_assert(std::is_pointer_v<T> || std::is_integral_v<T> || std::is_enum_v<T>);
	}
}

namespace KxFramework
{
	class KxThreadEvent: public wxThreadEvent
	{
		public:
			KxEVENT_MEMBER(KxThreadEvent, Execute);
			KxEVENT_MEMBER(KxThreadEvent, Started);
			KxEVENT_MEMBER(KxThreadEvent, Finished);

		private:
			wxThread::ExitCode m_ExitCode = nullptr;

		public:
			KxThreadEvent() = default;

		public:
			KxThreadEvent* Clone() const override
			{
				return new KxThreadEvent(*this);
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
