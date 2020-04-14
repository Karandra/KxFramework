#pragma once
#include "Common.h"
#include "Kx/EventSystem/Event.h"
#include "Kx/General/String.h"
#include "Kx/General/BinarySize.h"

namespace KxFramework
{
	class KX_API FileOperationEvent: public wxNotifyEvent
	{
		public:
			KxEVENT_DECLARE_MEMBER(FileOperationEvent, Copy);
			KxEVENT_DECLARE_MEMBER(FileOperationEvent, Move);
			KxEVENT_DECLARE_MEMBER(FileOperationEvent, Rename);
			KxEVENT_DECLARE_MEMBER(FileOperationEvent, Remove);
			KxEVENT_DECLARE_MEMBER(FileOperationEvent, Search);

		private:
			String m_Source;
			String m_Destination;
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
			wxString GetString() const = delete;
			void SetString(const wxString&) = delete;

			String GetCurrent() const
			{
				return wxNotifyEvent::GetString();
			}
			void SetCurrent(const String& string)
			{
				wxNotifyEvent::SetString(string);
			}
			
			String GetSource() const
			{
				return m_Source;
			}
			void SetSource(const String& string)
			{
				m_Source = string;
			}
			
			String GetDestination() const
			{
				return m_Destination;
			}
			void SetDestination(const String& string)
			{
				m_Destination = string;
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
