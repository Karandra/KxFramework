#pragma once
#include "Common.h"
#include <wx/event.h>

namespace kxf
{
	class IWidget;
}

namespace kxf::WXUI::Private
{
	class KX_API EvtHandlerWrapperBase final
	{
		private:
			IWidget& m_Widget;

		public:
			EvtHandlerWrapperBase(IWidget& widget)
				:m_Widget(widget)
			{
			}

		public:
			bool TryBefore(wxEvent& anyEvent);
			bool TryAfter(wxEvent& anyEvent);

			bool OnDynamicBind(wxDynamicEventTableEntry& eventItem);
	};
}

namespace kxf::WXUI
{
	template<class TBase>
	class EvtHandlerWrapper: public TBase
	{
		private:
			Private::EvtHandlerWrapperBase m_Wrapper;

		protected:
			bool TryBefore(wxEvent& event) override
			{
				if (m_Wrapper.TryBefore(event))
				{
					return true;
				}
				return TBase::TryBefore(event);
			}
			bool TryAfter(wxEvent& event) override
			{
				if (TBase::TryAfter(event))
				{
					return true;
				}
				return m_Wrapper.TryAfter(event);
			}

			bool OnDynamicBind(wxDynamicEventTableEntry& eventItem) override
			{
				return m_Wrapper.OnDynamicBind(eventItem) && TBase::OnDynamicBind(eventItem);
			}

		public:
			EvtHandlerWrapper(IWidget& widget)
				:m_Wrapper(widget)
			{
			}
	};
}
