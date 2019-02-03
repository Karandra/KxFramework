#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxDynamicCastAsIs.h"

class KX_API KxBroadcastEvent: public wxNotifyEvent, public KxRTTI::DynamicCastAsIs<KxBroadcastEvent>
{
	public:
		template<class TEvent, class TFunction, class THandler>
		static void Bind(const wxEventTypeTag<TEvent>& eventType, const TFunction& function, THandler handler)
		{
			wxTheApp->Bind(eventType, [function, handler](TEvent& event)
			{
				(handler->*function)(event);
				event.wxNotifyEvent::Skip();
			});
		}
		template<class TEvent, class TFunction, class THandler>
		static void Unbind(const wxEventTypeTag<TEvent>& eventType, const TFunction& function, THandler handler)
		{
			wxTheApp->Unbind(eventType, function, handler);
		}
		
		template<class TEvent, class TFunctor>
		static void Bind(const wxEventTypeTag<TEvent>& eventType, const TFunctor& functor)
		{
			wxTheApp->Bind(eventType, [functor](TEvent& event)
			{
				functor(event);
				event.wxNotifyEvent::Skip();
			});
		}
		template<class TEvent, class TFunctor>
		static void Unbind(const wxEventTypeTag<TEvent>& eventType, const TFunctor& functor)
		{
			wxTheApp->Unbind(eventType, functor);
		}

		template<class... Args>
		static void CallAfter(Args&&... arg)
		{
			wxTheApp->CallAfter(std::forward<Args>(arg)...);
		}

		template<class T, class... Args>
		static void MakeQueue(Args&&... arg)
		{
			T* event = new T(std::forward<Args>(arg)...);
			event->Queue();
		}

		template<class T, class... Args>
		static bool MakeSend(Args&&... arg)
		{
			return T(std::forward<Args>(arg)...).Send();
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
