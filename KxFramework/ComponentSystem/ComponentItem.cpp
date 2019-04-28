#include "KxStdAfx.h"
#include "ComponentItem.h"
#include "Component.h"

void KxComponentItem::Destroy()
{
	if (m_Component && m_NeedDelete)
	{
		m_Component->Delete();
	}
	MakeNull();
}
void KxComponentItem::MakeNull()
{
	m_Component = nullptr;
	m_TypeInfo = nullptr;
	m_NeedDelete = false;
}
