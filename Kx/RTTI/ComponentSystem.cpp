#include "stdafx.h"
#include "ComponentSystem.h"

namespace kxf::RTTI
{
	void ComponentItem::Destroy() noexcept
	{
		if (m_Component && m_NeedDelete)
		{
			m_Component->OnDestroy();
		}
	}
}
