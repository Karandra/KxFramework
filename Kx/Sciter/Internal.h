#pragma once
#include <wx/wx.h>
#include "SciterAPI.h"

namespace KxSciter
{
	struct NodeHandle;
	struct ElementHandle;
}

namespace KxSciter
{
	std::vector<BYTE> ToSciterUTF8(const wxString& text);

	inline HELEMENT ToSciterElement(ElementHandle* handle)
	{
		return reinterpret_cast<HELEMENT>(handle);
	}
	inline ElementHandle* FromSciterElement(HELEMENT handle)
	{
		return reinterpret_cast<ElementHandle*>(handle);
	}

	inline HNODE ToSciterNode(void* handle)
	{
		return reinterpret_cast<HNODE>(handle);
	}
	inline NodeHandle* FromSciterNode(HNODE handle)
	{
		return reinterpret_cast<NodeHandle*>(handle);
	}
}
