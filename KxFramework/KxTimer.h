/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxTimer: public wxTimer
{
	public:
		KxTimer() = default;
		KxTimer(wxEvtHandler* owner, int id = wxID_ANY)
			:wxTimer(owner, id)
		{
		}
};

//////////////////////////////////////////////////////////////////////////
template<class TFunctor> class KX_API KxTimerFunctor: public KxTimer
{
	private:
		TFunctor m_Functor;

	protected:
		virtual void Notify() override
		{
			wxTimerEvent event(*this);
			std::invoke(m_Functor, event);
		}

	public:
		KxTimerFunctor(const TFunctor& functor)
			:m_Functor(functor)
		{
		}
		KxTimerFunctor(TFunctor&& functor)
			:m_Functor(std::move(functor))
		{
		}
};

//////////////////////////////////////////////////////////////////////////
template<class T> class KX_API KxTimerMethod: public KxTimer
{
	private:
		using TMethod = void(T::*)(wxTimerEvent&);

	private:
		TMethod m_Function = nullptr;
		T* m_Object = nullptr;

	protected:
		virtual void Notify() override
		{
			if (m_Object && m_Function)
			{
				wxTimerEvent event(*this);
				std::invoke(m_Function, m_Object, event);
			}
		}

	public:
		KxTimerMethod() = default;
		KxTimerMethod(TMethod function, T* object)
			:m_Object(object), m_Function(function)
		{
		}

	public:
		void BindFunction(TMethod function, T* object)
		{
			m_Object = object;
			m_Function = function;
		}
};
