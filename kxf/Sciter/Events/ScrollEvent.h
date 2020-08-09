#pragma once
#include "SciterEvent.h"

namespace kxf::Sciter
{
	class KX_API ScrollEvent: public SciterEvent
	{
		public:
			KxEVENT_MEMBER(ScrollEvent, ScrollHome);
			KxEVENT_MEMBER(ScrollEvent, ScrollEnd);
			KxEVENT_MEMBER(ScrollEvent, ScrollPosition);

			KxEVENT_MEMBER(ScrollEvent, ScrollStepPlus);
			KxEVENT_MEMBER(ScrollEvent, ScrollStepMinus);
			KxEVENT_MEMBER(ScrollEvent, ScrollPagePlus);
			KxEVENT_MEMBER(ScrollEvent, ScrollPageMinus);

			KxEVENT_MEMBER(ScrollEvent, ScrollSliderPressed);
			KxEVENT_MEMBER(ScrollEvent, ScrollSliderReleased);

			KxEVENT_MEMBER(ScrollEvent, ScrollCornerPressed);
			KxEVENT_MEMBER(ScrollEvent, ScrollCornerReleased);

		protected:
			Orientation m_Orientation = Orientation::Vertical;
			ScrollSource m_Source = ScrollSource::Unknown;
			int m_Position = Geometry::DefaultCoord;

		public:
			ScrollEvent(Host& host)
				:SciterEvent(host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ScrollEvent>(std::move(*this));
			}

			// IEvent
			Orientation GetOrientation() const
			{
				return m_Orientation;
			}
			void SetOrientation(Orientation value)
			{
				m_Orientation = value;
			}
			
			int GetPosition() const
			{
				return m_Position;
			}
			void SetPosition(int value)
			{
				m_Position = value;
			}
			
			ScrollSource GetSource() const
			{
				return m_Source;
			}
			void SetSource(ScrollSource value)
			{
				m_Source = value;
			}
	};
}

namespace kxf::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollHome);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollEnd);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollPosition);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollStepPlus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollStepMinus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollPagePlus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollPageMinus);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollSliderPressed);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollSliderReleased);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollCornerPressed);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(ScrollEvent, ScrollCornerReleased);
}
