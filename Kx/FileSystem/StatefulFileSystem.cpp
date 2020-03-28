#include "KxStdAfx.h"
#include "StatefulFileSystem.h"

namespace KxFramework
{
	bool StatefulFileSystem::Navigate(const FSPath& path)
	{
		if (path)
		{
			if (path.IsAbsolute())
			{
				m_CurrentLocation = path;
			}
			else
			{
				m_CurrentLocation /= path;
			}
			return m_CurrentLocation.IsValid();
		}
		return false;
	}
	bool StatefulFileSystem::NavigateUp()
	{
		return Navigate(m_CurrentLocation.GetParent());
	}

	FSPath StatefulFileSystem::GetCurrentLocation() const
	{
		return m_CurrentLocation;
	}
}
