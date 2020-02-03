#include "KxStdAfx.h"
#include "Event.h"
#include "Kx/Sciter/Host.h"

namespace KxSciter
{
	Element Event::RetrieveElement(ElementUID* uid) const
	{
		return m_Host->GetElementByUID(uid);
	}
	void Event::AcceptElement(ElementUID*& uid, const Element& element)
	{
		uid = element.GetUID();
	}
}
