#pragma once
#include "KxFramework/KxFramework.h"

class KxTimer: public wxTimer
{
	public:
		KxTimer() = default;
		KxTimer(wxEvtHandler* owner, int id = wxID_ANY)
			:wxTimer(owner, id)
		{
		}
};

//////////////////////////////////////////////////////////////////////////
template<class Functor> class KxTimerFunctor: public KxTimer
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
template<class T> class KxTimerMethod: public KxTimer
{
	private:
		using FunctionSignature = void(T::*)();

	private:
		FunctionSignature m_Function = NULL;
		T* m_Object = NULL;

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
