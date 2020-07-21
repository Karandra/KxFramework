#pragma once
#include "Common.h"
#include "FSPath.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/General/String.h"
#include "kxf/General/BinarySize.h"

namespace kxf
{
	class KX_API FileOperationEvent: public BasicEvent
	{
		public:
			KxEVENT_MEMBER(FileOperationEvent, Copy);
			KxEVENT_MEMBER(FileOperationEvent, Move);
			KxEVENT_MEMBER(FileOperationEvent, Rename);
			KxEVENT_MEMBER(FileOperationEvent, Remove);
			KxEVENT_MEMBER(FileOperationEvent, Search);

		private:
			String m_String;
			FSPath m_Source;
			FSPath m_Destination;
			BinarySize m_Completed = 0;
			BinarySize m_Total = 0;
			BinarySize m_Speed = 0;

		public:
			FileOperationEvent() = default;

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<FileOperationEvent>(std::move(*this));
			}

			// Paths and status
			String GetString() const
			{
				return m_String;
			}
			void SetString(String value)
			{
				m_String = std::move(value);
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
			BinarySize GetTotal() const
			{
				return m_Total;
			}
			BinarySize GetCompleted() const
			{
				return m_Completed;
			}
			
			void SetTotal(BinarySize value)
			{
				m_Total = value;
			}
			void SetCompleted(BinarySize value)
			{
				m_Completed = value;
			}
			void SetProgress(BinarySize completed, BinarySize total)
			{
				m_Completed = completed;
				m_Total = total;
			}

			bool IsCompleted() const
			{
				return GetCompleted() >= GetTotal();
			}
			void MakeCompleted()
			{
				m_Completed = m_Total;
			}
			bool IsProgressKnown() const
			{
				return m_Completed.IsValid() && m_Total.IsValid();
			}
			double GetProgressRatio() const noexcept
			{
				return GetSizeRatio(m_Completed, m_Total);
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
	};
}
