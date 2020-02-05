#pragma once
#include <cstdint>
#include <wx/datetime.h>

namespace KxSciter
{
	class FPSCounter final
	{
		public:
			class Watcher final
			{
				private:
					FPSCounter& m_Counter;

				public:
					Watcher(FPSCounter& counter)
						:m_Counter(counter)
					{
						m_Counter.OnStartRender();
					}
					~Watcher()
					{
						m_Counter.OnEndRender();
					}
			};

		private:
			std::deque<int64_t> m_FrameTime;
			double m_AverageFrameCount = 0.0;
			wxTimeSpan m_TrackTime;

		public:
			FPSCounter(const wxTimeSpan& trackTime = wxTimeSpan::Milliseconds(1000))
				:m_TrackTime(trackTime)
			{
			}

		public:
			void OnStartRender();
			void OnEndRender();

			Watcher CreateWatcher()
			{
				return *this;
			}
			double GetCount() const
			{
				return m_AverageFrameCount;
			}
	};
}
