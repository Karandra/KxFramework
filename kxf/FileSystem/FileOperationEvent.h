#pragma once
#include "Common.h"
#include "FSPath.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/General/String.h"
#include "kxf/General/BinarySize.h"

namespace kxf
{
	class KX_API FileOperationEvent: public wxNotifyEvent
	{
		public:
			KxEVENT_MEMBER(FileOperationEvent, Copy);
			KxEVENT_MEMBER(FileOperationEvent, Move);
			KxEVENT_MEMBER(FileOperationEvent, Rename);
			KxEVENT_MEMBER(FileOperationEvent, Remove);
			KxEVENT_MEMBER(FileOperationEvent, Search);

		private:
			FSPath m_Source;
			FSPath m_Destination;
			BinarySize m_Processed = 0;
			BinarySize m_Total = 0;
			BinarySize m_Speed = 0;

		public:
			FileOperationEvent(EventID type = Event::EvtNull, int id = 0) noexcept
				:wxNotifyEvent(type, id)
			{
				Allow();
			}

		public:
			FileOperationEvent* Clone() const override
			{
				return new FileOperationEvent(*this);
			}

			// Stop operation
			bool IsStopped() const
			{
				return !wxNotifyEvent::IsAllowed();
			}
			void Stop()
			{
				wxNotifyEvent::Veto();
			}

			// Paths and status
			String GetString() const
			{
				return wxNotifyEvent::GetString();
			}
			void SetString(const String& value)
			{
				wxNotifyEvent::SetString(value);
			}

			FSPath GetSource() const
			{
				return m_Source;
			}
			void SetSource(FSPath source)
			{
				m_Source = std::move(source);
			}
			
			FSPath GetDestination() const
			{
				return m_Destination;
			}
			void SetDestination(FSPath destination)
			{
				m_Destination = std::move(destination);
			}

			// Size status
			BinarySize GetProcessed() const
			{
				return m_Processed;
			}
			void SetProcessed(BinarySize value)
			{
				m_Processed = value;
			}
			
			BinarySize GetTotal() const
			{
				return m_Total;
			}
			void SetTotal(BinarySize value)
			{
				m_Total = value;
			}

			bool IsOperationComplete() const
			{
				return GetProcessed() >= GetTotal();
			}
			void SetOperationComplete()
			{
				m_Processed = m_Total;
			}
			bool IsProgressKnown() const
			{
				return m_Processed.IsValid() && m_Total.IsValid();
			}
			double GetProgressRatio() const noexcept
			{
				return GetSizeRatio(m_Processed, m_Total);
			}

			// Speed
			bool IsSpeedKnown() const
			{
				return m_Speed.IsValid();
			}
			BinarySize GetSpeed() const noexcept
			{
				return m_Speed;
			}
			void SetSpeed(BinarySize value) noexcept
			{
				m_Speed = value;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(FileOperationEvent);
	};
}
