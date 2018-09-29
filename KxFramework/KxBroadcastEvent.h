#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxRTTI.h"

class KxBroadcastEvent: public wxNotifyEvent, public KxRTTI::DynamicCastAsIs<KxBroadcastEvent>
{
	public:
		template<class EventT, class Function, class HandlerT>
		static void Bind(const wxEventTypeTag<EventT>& eventType, const Function& function, HandlerT handler)
		{
			wxTheApp->Bind(eventType, [function, handler](EventT& event)
			{
				(handler->*function)(event);
				event.wxNotifyEvent::Skip();
			});
		}
		
		template<class EventT, class FunctorT>
		static void Bind(const wxEventTypeTag<EventT>& eventType, const FunctorT& functor)
		{
			wxTheApp->Bind(eventType, [&functor](EventT& event)
			{
				functor(event);
				event.wxNotifyEvent::Skip();
			});
		}

		template<class... Args> static void CallAfter(Args&&... args)
		{
			wxTheApp->CallAfter(std::forward<Args>(args)...);
		}

	protected:
		static bool SendEvent(wxEvent& event)
		{
			return wxTheApp->ProcessEvent(event);
		}
		static void QueueEvent(wxEvent* event)
		{
			wxTheApp->QueueEvent(event);
		}

	private:
		int StopPropagation() = delete;
		void ResumePropagation(int propagationLevel) = delete;
		
		void Skip(bool skip = true) = delete;
		bool GetSkipped() const = delete;

	public:
		KxBroadcastEvent(wxEventType type = wxEVT_NULL);
		virtual ~KxBroadcastEvent();
		KxBroadcastEvent* Clone() const override;

	public:
		bool Send()
		{
			return SendEvent(*this);
		}
		void Queue()
		{
			QueueEvent(this);
		}
		void QueueClone()
		{
			QueueEvent(Clone());
		}
};
