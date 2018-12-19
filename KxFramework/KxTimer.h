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
template<class Functor> class KX_API KxTimerFunctor: public KxTimer
{
	private:
		Functor m_Functor;

	protected:
		virtual void Notify() override
		{
			std::invoke(m_Functor);
		}

	public:
		KxTimerFunctor(const Functor& functor)
			:m_Functor(functor)
		{
		}
		KxTimerFunctor(Functor&& functor)
			:m_Functor(std::move(functor))
		{
		}
};

//////////////////////////////////////////////////////////////////////////
template<class T> class KX_API KxTimerMethod: public KxTimer
{
	private:
		using FunctionSignature = void(T::*)();

	private:
		FunctionSignature m_Function = nullptr;
		T* m_Object = nullptr;

	protected:
		virtual void Notify() override
		{
			std::invoke(m_Function, m_Object);
		}

	public:
		KxTimerMethod() = default;
		KxTimerMethod(FunctionSignature function, T* object)
			:m_Object(object), m_Function(function)
		{
		}

	public:
		void BindFunction(FunctionSignature function, T* object)
		{
			m_Object = object;
			m_Function = function;
		}
};
