#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxRTTI.h"

class KX_API KxBroadcastEvent: public wxNotifyEvent, public KxRTTI::DynamicCastAsIs<KxBroadcastEvent>
{
	public:
		template<class EventT, class FunctionT, class HandlerT>
		static void Bind(const wxEventTypeTag<EventT>& eventType, const FunctionT& function, HandlerT handler)
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

		template<class... Args>
		static void CallAfter(Args&&... args)
		{
			wxTheApp->CallAfter(std::forward<Args>(args)...);
		}

		template<class T, class... Args>
		static void MakeQueue(Args&&... args)
		{
			T* event = new T(std::forward<Args>(args)...);
			event->Queue();
		}

		template<class T, class... Args>
		static bool MakeSend(Args&&... args)
		{
			return T(std::forward<Args>(args)...).Send();
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

	protected:
		virtual bool OnSendEvent()
		{
			return true;
		}

	public:
		KxBroadcastEvent(wxEventType type = wxEVT_NULL);
		virtual ~KxBroadcastEvent();
		KxBroadcastEvent* Clone() const override;

	public:
		bool Send()
		{
			if (OnSendEvent())
			{
				return SendEvent(*this);
			}
			return false;
		}
		void Queue()
		{
			if (OnSendEvent())
			{
				QueueEvent(this);
			}
		}
};
