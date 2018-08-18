#include "KxStdAfx.h"
#include "KxFramework/KxWithImageList.h"
#include "KxFramework/KxImageList.h"

void KxWithImageList::DeleteIfNeeded()
{
	if (m_IsOwned)
	{
		delete m_ImageList;
	}
}

KxWithImageList::KxWithImageList()
{
}
KxWithImageList::~KxWithImageList()
{
	DeleteIfNeeded();
}

void KxWithImageList::SetImageList(const KxImageList* imageList)
{
	DeleteIfNeeded();
	m_IsOwned = false;
	m_ImageList = const_cast<KxImageList*>(imageList);
}
void KxWithImageList::AssignImageList(KxImageList* imageList)
{
	DeleteIfNeeded();
	m_IsOwned = true;
	m_ImageList = imageList;
}
