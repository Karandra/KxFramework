#include "stdafx.h"
#include "ComponentSystem.h"

namespace KxFramework::RTTI
{
	void ComponentItem::Destroy() noexcept
	{
		if (m_Component && m_NeedDelete)
		{
			m_Component->OnDestroy();
		}
	}
}
