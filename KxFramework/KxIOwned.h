#pragma once
#include "KxFramework/KxFramework.h"

class KxIOwned
{
	public:
		virtual ~KxIOwned()
		{
		}

		virtual bool IsOwned() const = 0;
		virtual void Disown() = 0;
		virtual void TakeOwnership() = 0;
};

class KxIOwnedSimple: public KxIOwned
{
	private:
		bool m_IsOwned = false;

	protected:
		KxIOwnedSimple(bool isOwned)
			:m_IsOwned(isOwned)
		{
		}

	public:
		virtual bool IsOwned() const override
		{
			return m_IsOwned;
		}
		virtual void Disown() override
		{
			m_IsOwned = false;
		}
		virtual void TakeOwnership() override
		{
			m_IsOwned = true;
		}
};
