/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxIOwned
{
	public:
		virtual ~KxIOwned()
		{
		}

		virtual bool IsOwned() const = 0;
		virtual void Disown() = 0;
		virtual void TakeOwnership() = 0;
};

class KX_API KxIOwnedSimple: public KxIOwned
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
